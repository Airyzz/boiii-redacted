#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include <game/game.hpp>
#include <game/utils.hpp>

#include <utils/hook.hpp>
#include <utils/io.hpp>

#include <mmeapi.h>
#include <component/command.hpp>

namespace demo_patches
{
	namespace
	{
		utils::hook::detour demo_start_record_hook;
		utils::hook::detour demo_is_enabled_hook;
		utils::hook::detour demo_add_anim_hook;

		__int64 demo_start_record_f_stub()
		{
			__int64 result = demo_start_record_hook.invoke<__int64>();
			return result;
		}

		struct demo_anim_bundled
		{
			int id;
			int unk1;
			int unk2;
		};

		std::vector<demo_anim_bundled> demo;

		__int64 __fastcall demo_add_anim_stub(int a1, int a2)
		{
			int v2 = *(int*)(0x1594566F0_g);
			for (auto& data : demo) {
				if ((data).id == a1 && (data).unk1 == v2) {
					return (__int64)0x15943FCDC_g;
				}
			}
			demo_anim_bundled anim;
			anim.id = a1; anim.unk1 = v2; anim.unk2 = a2;
			demo.emplace_back(anim); 
			return demo_add_anim_hook.invoke<__int64>(a1, a2);
		}

		void __fastcall sub_141433370(const char* a1, __int64 a2, int a3)
		{
			int v3; // eax
			int v4; // ebx
			__int64 v5; // rsi
			char* v7; // rbx

			__int8* byte_159445CE0 = (__int8*)0x159445CE0_g;

			v3 = *(int*)0x1594466E0_g;
			v4 = 0;
			v5 = a3;
			if (*(int*)0x1594466E0_g <= 0)
			{
			LABEL_4:
				v7 = &byte_159445CE0[80 * v3];
				game::sub_1422E9410(v7, 64i64, (__int64)a1);
				*((int*)v7 + 16) = v5;
				*((__int64*)v7 + 9) = *(__int64*)0x1594566E8_g;
				game::sub_142C3D960();
				*(__int64*))0x1594566E8_g += v5;
				++*(int*)0x1594466E0_g;
			}
			else
			{
				while ((unsigned int)stricmp(a1, &byte_159445CE0[80 * v4]))
				{
					v3 = *(int*)0x1594466E0;
					if (++v4 >= *(int*)0x1594466E0)
						goto LABEL_4;
				}
			}
		}

		__int64 __fastcall sub_141433460(__int64 a1, __int64 a2)
		{
			unsigned int v4; // eax
			__int64 result; // rax
			int v6; // ebp
			int* v7; // r14
			__int64 v8; // rdi
			__int64 v9; // rbx
			unsigned int v10; // ebx

			game::MSG_WriteLong(a1, *(int*)(a2 + 0x7180));
			if ((*(int*)0x1499A7798_g & 1) != 0)
			{
				v4 = *(unsigned int*)0x1499A7794_g;
			}
			else
			{
				*(unsigned int*)0x1499A7798_g |= 1u;
				v4 = game::BB_RegisterHighWaterMark("demo_archived_animtree_count");
				*(unsigned int*)0x1499A7794_g = v4;
			}
			result = game::BB_SetHighWaterMark(v4, *(unsigned int*)(a2 + 0x7180));
			v6 = 0;
			printf("[sub_141433460] count %lld\n", *(int*)(a2 + 0x7180));
			if (*(int*)(a2 + 0x7180) > 0)
			{
				v7 = (int*)(a2 + 0x67C0);
				do
				{
					v8 = a2 + 80 * v6;
					v9 = -1i64;
					do
						++v9;
					while (*(__int16*)(v8 + v9 + 0x6780));
					v10 = v9 + 1;
					game::MSG_WriteLong(a1, (int)v10);
					game::sub_1421577E0(a1, (const char*)(v8 + 0x6780), v10);
					game::MSG_WriteLong(a1, *v7);
					printf("[sub_141433460] %lld : %s : %lld %s\n", v6, (const char*)(v8 + 0x6780), *v7, (const char*)*((__int64*)v7 + 1));
					result = game::sub_1421577E0(a1, (const char*)*((__int64*)v7 + 1), *v7);
					++v6;
					v7 += 20;
				} while (v6 < *(int*)(a2 + 0x7180));
			}
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
				v4 = (unsigned int)game::BB_RegisterHighWaterMark("demo_bundled_anims_count");
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
					game::sub_1421726D0(a1, (unsigned short)demo[v6].unk1);
					v9 = (__int64)game::sub_1412D7160((demo[v6].id));
					v10 = v9;
					if (v9)
					{
						v8 = -1i64;
						do
						{
							++v8;
						} while (*(byte*)(v9 + v8));
					}

					game::sub_1421726D0(a1, (unsigned short)v8);
					if ((int)v8 > 0)
					{
						game::sub_1421577E0(a1, (const char*)v10, (unsigned int)v8);
					}
					result = game::sub_1421576F0(a1, (demo[v6].unk2), 3);
					++v6;
					v7 += 3;
				} while (v6 < *(int*)(a2 + 0x778));
			}
			demo.clear();
			return result;
		}

		bool demo_is_enable_stub()
		{
			return true;
		}

		game::fileHandle_t demo_open_file_read_stub(const char* filename, const char* dir, bool supressErrors)
		{
			game::fileHandle_t handle = game::FS_FOpenFileReadFromDir(filename, "boiii_players/user/demos", std::filesystem::current_path().string().c_str());
			return handle;
		}
	}

	class component final : public client_component
	{
	public:
		component()
		{
		}

		void post_unpack() override
		{

			// For no reason, multiplayer demos turn off after the first kill
			// nop CCS_ValidateChecksums for mp demo 
			utils::hook::nop(0x141365159_g, 5);

			// nop  if ( target < 0 || target >= com_maxclients )
			utils::hook::nop(0x1407F2055_g, 6);
			utils::hook::nop(0x1407F205D_g, 2);
			utils::hook::call(0x141433F08_g, sub_141433790);
			utils::hook::call(0x141433F13_g, sub_141433460);

			//Demo_OpenFileRead
		//	utils::hook::call(0x1426013CE_g, demo_open_file_read_stub); // ^^

			demo_start_record_hook.create(0x142646BD0_g, demo_start_record_f_stub);
			demo_is_enabled_hook.create(0x142600120_g, demo_is_enable_stub);


			command::add("demo_record", game::Demo_StartRecord_f);
			demo_add_anim_hook.create(0x141433300_g, demo_add_anim_stub);
		}
	};
}

REGISTER_COMPONENT(demo_patches::component)
