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
		utils::hook::detour sub_141433370_hook;

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

		std::vector<demo_anim_bundled> demo_bundled;

		__int64 __fastcall demo_add_anim_stub(int a1, int a2)
		{
			int v2 = *(int*)(0x1594566F0_g);
			for (auto& data : demo_bundled) {
				if ((data).id == a1 && (data).unk1 == v2) {
					return (__int64)0x15943FCDC_g;
				}
			}
			demo_anim_bundled anim;
			anim.id = a1; anim.unk1 = v2; anim.unk2 = a2;
	//		printf("%lld : %lld : %lld\n", a1, v2, a2);
			demo_bundled.emplace_back(anim);
			return demo_add_anim_hook.invoke<__int64>(a1, a2);
		}


		struct demo_archived_animtree
		{
			std::string name;
			int unk1;
			__int64 unk2;
		};

		std::vector<demo_archived_animtree> demo_animtree;

		void __fastcall sub_141433370(const char* a1, __int64 a2, int a3)
		{
			demo_archived_animtree animtree;
			animtree.name = a1; animtree.unk1 = a3; animtree.unk2 = *(__int64*)0x1594566E8_g;
				for (auto& data : demo_animtree) {
					if (data.name == std::string(a1) && (data).unk1 == a3) {
						return sub_141433370_hook.invoke(a1, a2, a3);
					}
				}
		//		printf("animtree %s : %lld : %lld : %lld\n", a1, a2, a3, *(__int64*)0x1594566E8_g);
			demo_animtree.emplace_back(animtree);
			sub_141433370_hook.invoke(a1, a2, a3);
		}

		__int64 __fastcall sub_141433460(__int64 a1, __int64 a2)
		{
			unsigned int v4; // eax
			__int64 result; // rax
			int v6; // ebp
			int* v7; // r14

			int size = (int)demo_animtree.size();
			game::MSG_WriteLong(a1, size);
			if ((*(int*)0x1499A7798_g & 1) != 0)
			{
				v4 = *(unsigned int*)0x1499A7794_g;
			}
			else
			{
				*(unsigned int*)0x1499A7798_g |= 1u;
				v4 = (unsigned int)game::BB_RegisterHighWaterMark("demo_archived_animtree_count");
				*(unsigned int*)0x1499A7794_g = v4;
			}
			result = game::BB_SetHighWaterMark(v4, size);
			v6 = 0;
			if (demo_animtree.size() > 0)
			{
				v7 = (int*)(a2 + 0x67C0);
				do
				{
					int strsize = (int)demo_animtree[v6].name.size();
					game::MSG_WriteLong(a1, strsize+1);
					game::sub_1421577E0(a1, demo_animtree[v6].name.c_str(), strsize+1);
					game::MSG_WriteLong(a1, demo_animtree[v6].unk1);
					result = game::sub_1421577E0(a1, (const char*)demo_animtree[v6].unk2, demo_animtree[v6].unk1);
					++v6;
					v7 += 20;
				} while (v6 < size);
			}
			demo_animtree.clear();
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
					game::sub_1421726D0(a1, (unsigned short)demo_bundled[v6].unk1);
					v9 = (__int64)game::SL_ConvertToString((demo_bundled[v6].id));
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
					result = game::sub_1421576F0(a1, (demo_bundled[v6].unk2), 3);
					++v6;
					v7 += 3;
				} while (v6 < *(int*)(a2 + 0x778));
			}
			demo_bundled.clear();
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

		//write anim_tree
		__int64 __fastcall sub_1414326E0(int* a1, __int64 a2)
		{
			__int64 result; // rax
			int v5; // ebp
			int* v6; // rdi
			int v7; // eax
			int v8; // eax
			__int64 v9; // rdx

			*(__int64*)(a2 + 94600) = a2 + 29060;
			result = game::MSG_ReadLong(a1);
			v5 = 0;
			*(int*)(a2 + 29056) = result;
		//	printf("count ? = %lld\n", result);
			if ((int)result > 0)
			{
				v6 = (int*)(a2 + 26560);
				do
				{
					v7 = game::MSG_ReadLong(a1);
					game::sub_1421554B0(a1, a2 + 80 * v5 + 26496, v7);
					v8 = game::MSG_ReadLong(a1);
					*v6 = v8;
					v9 = *(__int64*)(a2 + 94600);
					*((__int64*)v6 + 1) = v9;
					game::sub_1421554B0(a1, v9, v8);
					result = *v6;
					*(__int64*)(a2 + 94600) += result;
					++v5;
			//		printf("%lld : %lld : %lld : %lld : %lld \n", v5, result, v9, v7, v8);
					v6 += 20;
				} while (v5 < *(int*)(a2 + 29056));
			}
			return result;
		}

		//wrie anim_bundled
		unsigned __int64 __fastcall sub_141432B00(int* a1, __int64 a2)
		{
			unsigned __int64 result; // rax
			int v5; // ebp
			int* v6; // rsi
			__int16 v7; // ax
			unsigned __int64 v8; // rbx
			int v9; // eax
			char v10[256]; // [rsp+20h] [rbp-128h] BYREF
			demo_bundled.clear();
			result = game::MSG_ReadLong(a1);
			v5 = 0;
			*(int*)(a2 + 1912) = result;
			if ((int)result > 0)
			{
				v6 = (int*)(a2 + 1920);
				do
				{
					*(v6 - 1) = (__int16)game::sub_142156F30((__int64)a1);
					v7 = game::sub_142156F30((__int64)a1);
					v8 = v7;
					int unk1 = *(v6 - 1);
					if (v7 > 0)
						game::sub_1421554B0(a1, (__int64)v10, v7);
		
					v10[v8] = 0;
					v9 = game::SL_GetString_((__int64)v10, 1i64, 11i64);

					bool is_ = true;
					*v6 = v9;
					result = game::sub_142155300((__int64)a1, 3);
					++v5;
					v6 += 3;
					*(v6 - 2) = result;

					demo_anim_bundled anim;
					anim.id = v9;
					anim.unk1 = unk1;
					anim.unk2 = *(v6 - 2);
					for (auto data : demo_bundled) {
						if (data.id == v9) {
							is_ = false;
						}
					}
					if (is_)
						demo_bundled.emplace_back(anim);

			//		printf("%d : %d : %d : %lld\n", v5, *(v6 - 1), v9, result);
				} while (v5 < *(int*)(a2 + 1912));
			}
			return result;
		}

		//read anim_bundled
		int* __fastcall sub_1412C6AA0(unsigned int a1, unsigned int a2, int a3)
		{
			int v3; // edi
			int* result; // rax
			int* v9; // rbx
			int i; // er15
			int v11; // ecx
			int v12; // ecx
			int v29; // [rsp+A0h] [rbp+18h]
			int* v30; // [rsp+A8h] [rbp+20h]

			v29 = a3;
			v3 = a3;
			result = (int*)game::sub_1414323D0();
		//	printf("count = %lld\n", *result);
			v30 = result;
			v9 = result + 1;
			int size = (int)demo_bundled.size();
			auto demo_b = demo_bundled;
			for (i = 0; i < size; v9 += 3)
			{
				int q, b, c;
				q = demo_b[i].id;
				b = demo_b[i].unk1;
				c = demo_b[i].unk2;
			//		q = v9[1];
			//		b = *v9;
			//		c = v9[2];

			//	printf("%d : %d : %d : %d : %d\n",i, v3, *v9, v9[2], v9[1]);
			//	printf("%d : %d : %d : %d : %d\n",i, v3, b, c, q);
				if (b == v3)
				{
					v11 =c;
					if (v11)
					{
						v12 = v11 - 1;
						if (v12)
						{
							if (v12 == 1)
								game::sub_1412C4800(a1, a2, q);
						}
						else
						{
							if ((unsigned __int8)game::sub_142600220())
								demo_add_anim_hook.invoke<__int64>(q, 1);
							game::sub_1412C4540(a1, a2, q, *(int*)0x14A72A088_g, 96);
						}
					}
					else
					{
						if ((unsigned __int8)game::sub_142600220())
							demo_add_anim_hook.invoke<__int64>(q, 0);
						game::sub_1412C4540(a1, a2, q, *(int*)0x14A72A084_g, 0);
					}
					char* v22 = (char*)0x1450DC2D0_g;
					//__int64 v19 = (__int64)&v22[28 * q];
					__int64 v19 = (__int64)(volatile signed __int32*)((char*)0x1450DC2D0_g + 28 * q);;
				//	game::sub_1412D7E90(q, v19);
					game::sub_1412D7E90(q);
				}
				++i;
			}
	//		demo_bundled.clear();
			return result;
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

			//load demo

			utils::hook::call(0x1414332B7_g, sub_1414326E0);
			utils::hook::call(0x1412C4C71_g, sub_1412C6AA0);
			utils::hook::call(0x1414332AC_g, sub_141432B00);
	//		utils::hook::call(0x1412D4DF5_g, sub_141433370);

			//Demo_OpenFileRead
		//	utils::hook::call(0x1426013CE_g, demo_open_file_read_stub); // ^^

			demo_start_record_hook.create(0x142646BD0_g, demo_start_record_f_stub);
			demo_is_enabled_hook.create(0x142600120_g, demo_is_enable_stub);


			command::add("demo_record", game::Demo_StartRecord_f);
			demo_add_anim_hook.create(0x141433300_g, demo_add_anim_stub);
			sub_141433370_hook.create(0x141433370_g, sub_141433370);

		//	utils::hook::nop(0x1412C6BE4_g, 24);
		//	utils::hook::nop(0x1412C73F3_g, 40);
		//	utils::hook::nop(0x1412C4FF7_g, 5);
		}
	};
}

REGISTER_COMPONENT(demo_patches::component)
