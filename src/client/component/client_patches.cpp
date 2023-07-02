#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "scheduler.hpp"

#include <game/game.hpp>
#include <game/utils.hpp>

#include <utils/hook.hpp>
#include <utils/io.hpp>

#include <mmeapi.h>
#include "command.hpp"

namespace client_patches
{
	namespace
	{
		utils::hook::detour preload_map_hook;
		utils::hook::detour demo_start_record_hook;
		utils::hook::detour demo_is_enabled_hook;

		const game::dvar_t* cl_yaw_speed;
		const game::dvar_t* cl_pitch_speed;

		void stop_intro_if_needed()
		{
			if (game::Com_SessionMode_GetMode() != game::MODE_ZOMBIES &&
			    game::Com_SessionMode_GetMode() != game::MODE_CAMPAIGN)
			{
				return;
			}

			scheduler::once([]
			{
				scheduler::schedule([]
				{
					if (!game::Sys_IsDatabaseReady())
					{
						return scheduler::cond_continue;
					}

					game::Cinematic_StopPlayback(0, true);
					return scheduler::cond_end;
				}, scheduler::main);
			}, scheduler::main, 15s);
		}

		void preload_map_stub(int local_client_num, const char* mapname, const char* gametype)
		{
			if (game::isModLoaded())
			{
				//for Theater TODO
				std::filesystem::path path_to_map_folder = "usermaps";
				if (std::filesystem::exists(path_to_map_folder / mapname)) {
					utils::hook::copy_string(0x1567D9A24_g, mapname);
				}
			}
			game::Com_GametypeSettings_SetGametype(gametype, true);
			stop_intro_if_needed();
			preload_map_hook.invoke(local_client_num, mapname, gametype);
		}

		__int64 demo_start_record_f_stub()
		{
			__int64 result = demo_start_record_hook.invoke<__int64>();
			return result;
		}

		__int64 __fastcall sub_141433790(__int64 a1, __int64 a2)
		{
			unsigned int v4; // eax
			__int64 result; // rax
			int v6; // er14
			unsigned int* v7; // rsi
			__int64 v8; // rbx
			__int64 v9; // rax
			__int64 v10; // rdi

			game::MSG_WriteLong(a1, *(int*)(a2 + 1912));
			if ((*(int*)(0x1499A77A0_g) & 1) != 0)
			{
				v4 = *(unsigned int*)(0x1499A779C_g);
			}
			else
			{
				*(int*)(0x1499A77A0_g) |= 1u;
				v4 = game::BB_RegisterHighWaterMark("demo_bundled_anims_count");
				*(unsigned int*)(0x1499A779C_g) = v4;
			}
			result = game::sub_14229EB90(v4, *(unsigned int*)(a2 + 1912));
			v6 = 0;
			v8 = 0;
			if (*(int*)(a2 + 1912) > 0)
			{
				v7 = (unsigned int*)(a2 + 1920);
				do
				{
					v8 &= 0xFFFFFFFF00000000;
					game::sub_1421726D0(a1, (int)*(v7 - 1));
					v9 = (__int64)game::sub_1412D7160(*v7);
					v10 = v9;
				//	printf("%lld : %lld : %lld\n", v6, v7, v9);
					if (v9 && !IsBadReadPtr((void*)(v9), sizeof(byte)))
					{
						v8 = -1i64;
						do
						{
							++v8;
						} while (*(byte*)(v9 + v8));
					}

					game::sub_1421726D0(a1, (int)v8);
					if ((int)v8 > 0)
					{
						game::sub_1421577E0(a1, v10, (unsigned int)v8);
					}
					result = game::sub_1421576F0(a1, (int)v7[1], 3);
					++v6;
					v7 += 3;
				} while (v6 < *(DWORD*)(a2 + 0x778));
			}
			return result;
		}

		bool demo_is_enable_stub()
		{
			return true;
		}

		void reduce_process_affinity()
		{
			const DWORD_PTR affinity = (1ULL << (std::min(std::thread::hardware_concurrency(), 4U))) - 1;
			SetProcessAffinityMask(GetCurrentProcess(), affinity);
		}

		void reset_process_affinity()
		{
			DWORD_PTR affinity_proc, affinity_sys;
			GetProcessAffinityMask(GetCurrentProcess(), &affinity_proc, &affinity_sys);
			SetProcessAffinityMask(GetCurrentProcess(), affinity_sys);
		}

		void fix_amd_cpu_stuttering()
		{
			scheduler::once([]
			{
				reduce_process_affinity();
				scheduler::once(reset_process_affinity, scheduler::pipeline::main, 1s);
			}, scheduler::pipeline::main);
		}

		MMRESULT mixer_open_stub()
		{
			return MMSYSERR_NODRIVER;
		}

		bool is_mod_loaded_stub()
		{
			return false;
		}

		void patch_is_mod_loaded_checks()
		{
			const std::vector<uintptr_t> is_mod_loaded_addresses =
			{
				{ 0x1420F7484_g },
				{ 0x1420F74A4_g },
				{ 0x1420F73E4_g },
				{ 0x1420F73B4_g },
				{ 0x1420F6E57_g },
				{ 0x1413E6A54_g },
				{ 0x1415E7EBB_g },
				{ 0x1415E87BB_g },
				{ 0x1415EBAC9_g },
				{ 0x1415F1F09_g },
				{ 0x1415F1FB9_g },
				{ 0x1415F2080_g },
				{ 0x1415F7F40_g },
				{ 0x141A8D0ED_g },
				{ 0x141AA70F9_g },
				{ 0x141EA06FB_g },
				{ 0x141EA8C7E_g },
				{ 0x141EB1A39_g },
				{ 0x141ECBA9D_g },
				{ 0x1420F6E1D_g },
			};

			for (const auto& address : is_mod_loaded_addresses)
			{
				utils::hook::call(address, is_mod_loaded_stub);
			}
		}

		float cl_key_state_yaw_speed_stub(void* key)
		{
			return game::CL_KeyState(key) * cl_yaw_speed->current.value.value;
		}

		float cl_key_state_pitch_speed_stub(void* key)
		{
			return game::CL_KeyState(key) * cl_pitch_speed->current.value.value;
		}

		game::fileHandle_t fs_f_open_file_write_to_dir_stub(const char* filename, [[maybe_unused]] const char* dir,
		                                                    const char* os_base_path)
		{
			return game::FS_FOpenFileWriteToDir(filename, "boiii_players", os_base_path);
		}

		game::fileHandle_t fs_f_open_file_read_from_dir_stub(const char* filename, [[maybe_unused]] const char* dir,
		                                                     const char* os_base_path)
		{
			return game::FS_FOpenFileReadFromDir(filename, "boiii_players", os_base_path);
		}

		int i_stricmp_stub(const char* s0, [[maybe_unused]] const char* s1)
		{
			return game::I_stricmp(s0, "boiii_players");
		}

		void fs_add_game_directory_stub(const char* path, [[maybe_unused]] const char* dir)
		{
			utils::hook::invoke<void>(0x1422A2AF0_g, path, "boiii_players");
		}

		// TODO: Remove me after some time
		extern "C" void migrate_if_needed()
		{
			std::error_code e;

			// Folder does not exist. Nothing to migrate
			if (!std::filesystem::is_directory("players", e))
			{
				return;
			}

			// Folder does exist. Already migrated
			if (std::filesystem::is_directory("boiii_players", e))
			{
				return;
			}

			utils::io::create_directory("boiii_players");

			std::filesystem::copy("players", "boiii_players", std::filesystem::copy_options::recursive, e);
		}

		game::fileHandle_t demo_open_file_read_stub(const char* filename, const char* dir, bool supressErrors)
		{
			game::fileHandle_t handle = game::FS_FOpenFileReadFromDir(filename, "boiii_players/user/demos", std::filesystem::current_path().string().c_str());
			return handle;
		}

		void patch_players_folder_name()
		{
			// Override 'players' folder
			utils::hook::call(0x14134764F_g, fs_f_open_file_write_to_dir_stub); // ??
			utils::hook::set<uint8_t>(0x14134762E_g, 0xEB); // ^^

			utils::hook::call(0x1413477EE_g, fs_f_open_file_write_to_dir_stub); // ??
			utils::hook::set<uint8_t>(0x1413477CD_g, 0xEB); // ^^

			utils::hook::call(0x141C20A1F_g, fs_f_open_file_write_to_dir_stub); // ??
			utils::hook::set<uint8_t>(0x141C209FE_g, 0xEB); // ^^

			utils::hook::call(0x1422F391E_g, fs_f_open_file_write_to_dir_stub); // ??

			utils::hook::call(0x141C2090F_g, fs_f_open_file_read_from_dir_stub); // ??
			utils::hook::set<uint8_t>(0x141C208EE_g, 0xEB); // ^^

			utils::hook::call(0x1422F3773_g, fs_f_open_file_read_from_dir_stub); // ??

			utils::hook::call(0x1422A2A61_g, i_stricmp_stub); // ??
			utils::hook::call(0x1422A2C82_g, i_stricmp_stub); // FS_AddGameDirectory

			utils::hook::call(0x1422A45A4_g, fs_add_game_directory_stub); // FS_Startup
		}
	}

	class component final : public client_component
	{
	public:
		static_assert(offsetof(game::clientActive_t, viewangles) == 0xB8C8);

		component()
		{
			migrate_if_needed(); // TODO: Remove me after some time
		}

		void post_unpack() override
		{
			fix_amd_cpu_stuttering();

			// Don't modify process priority
			utils::hook::nop(0x142334C98_g, 6);

			// For no reason, multiplayer demos turn off after the first kill
			{
				// nop CCS_ValidateChecksums for mp demo 
				utils::hook::nop(0x141365159_g, 5);

				// nop  if ( target < 0 || target >= com_maxclients )
				utils::hook::nop(0x1407F2055_g, 6);
				utils::hook::nop(0x1407F205D_g, 2);
				utils::hook::call(0x141433F08_g, sub_141433790);

				//Demo_OpenFileRead
			//	utils::hook::call(0x1426013CE_g, demo_open_file_read_stub); // ^^

				demo_start_record_hook.create(0x142646BD0_g, demo_start_record_f_stub);
				demo_is_enabled_hook.create(0x142600120_g, demo_is_enable_stub);

				command::add("demo_record", game::Demo_StartRecord_f);
			}
			// Kill microphones for now
			utils::hook::set(0x15AAE9254_g, mixer_open_stub);

			preload_map_hook.create(0x14135A1E0_g, preload_map_stub);

			// Keep client ranked when mod loaded
			utils::hook::jump(0x1420D5BA0_g, is_mod_loaded_stub);
			patch_is_mod_loaded_checks();

			// Kill Client/Server Index Mismatch error
			utils::hook::set<uint8_t>(0x1400A7588_g, 0xEB);

			// Always get loadscreen gametype from s_gametype
			utils::hook::set<uint8_t>(0x14228F5DC_g, 0xEB);

			cl_yaw_speed = game::register_dvar_float("cl_yawspeed", 140.0f, std::numeric_limits<float>::min(), std::numeric_limits<float>::max(),
			                                         game::DVAR_NONE, "Max yaw speed in degrees for game pad and keyboard");
			cl_pitch_speed = game::register_dvar_float("cl_pitchspeed", 140.0f, std::numeric_limits<float>::min(), std::numeric_limits<float>::max(),
			                                           game::DVAR_NONE, "Max pitch speed in degrees for game pad");
			// CL_AdjustAngles
			utils::hook::call(0x1412F3324_g, cl_key_state_yaw_speed_stub); // cl_yawspeed
			utils::hook::call(0x1412F3344_g, cl_key_state_yaw_speed_stub); // ^^

			utils::hook::call(0x1412F3380_g, cl_key_state_pitch_speed_stub); // cl_pitchspeed
			utils::hook::call(0x1412F33A1_g, cl_key_state_pitch_speed_stub); // ^^

			patch_players_folder_name();
		}
	};
}

REGISTER_COMPONENT(client_patches::component)
