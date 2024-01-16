#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include <discord_rpc.h>

#include "scheduler.hpp"

namespace discord
{
	namespace
	{
		void joinRequest(const DiscordUser* request)
		{
			SetEnvironmentVariableA("discord_user", request->userId);

			printf("Discord: joinRequest: %s - %s\n", request->userId, request->username);

			DiscordRichPresence discord_presence{};
			ZeroMemory(&discord_presence, sizeof(discord_presence));

			discord_presence.instance = 1;
			discord_presence.state = "BOIII";

			discord_presence.partySize = rand() % 100 + 1;
			discord_presence.partyMax = rand() % 100 + 1;
			discord_presence.startTimestamp = rand() % 1000000;

			Discord_UpdatePresence(&discord_presence);
		}
		void ready(const DiscordUser* request)
		{
			SetEnvironmentVariableA("discord_user", request->userId);

			printf("Discord: Ready: %s - %s\n", request->userId, request->username);

			DiscordRichPresence discord_presence{};
			ZeroMemory(&discord_presence, sizeof(discord_presence));

			discord_presence.instance = 1;
			discord_presence.state = "BOIII";

			discord_presence.partySize = 0;
			discord_presence.partyMax = 0;
			discord_presence.startTimestamp = 0;

			Discord_UpdatePresence(&discord_presence);
		}

		void errored(const int error_code, const char* message)
		{
			printf("Discord: Error (%i): %s\n", error_code, message);
		}

		void update(const char* joinSecret)
		{
			printf("Discord: update: %s - %s\n", joinSecret, joinSecret);

			DiscordRichPresence discord_presence{};
			ZeroMemory(&discord_presence, sizeof(discord_presence));

			discord_presence.instance = rand() % 100 + 1;
			discord_presence.state = "BOIII";

			discord_presence.partySize = rand() % 100 + 1;
			discord_presence.partyMax = rand() % 100 + 1;
			discord_presence.startTimestamp = rand() % 1000000;

			Discord_UpdatePresence(&discord_presence);
		}
	}

	class component final : public client_component
	{
	public:
		void post_load() override
		{
			DiscordEventHandlers handlers;
			ZeroMemory(&handlers, sizeof(handlers));
			handlers.ready = ready;
			handlers.errored = errored;
			handlers.disconnected = errored;
			handlers.joinGame = update;
			handlers.spectateGame = update;
			handlers.joinRequest = joinRequest;

			Discord_Initialize("1047539933922988112", &handlers, 1, nullptr);

			this->initialized_ = true;

			scheduler::loop(Discord_RunCallbacks, scheduler::pipeline::async, 1s);
		}

		void pre_destroy() override
		{
			if (this->initialized_)
			{
				Discord_Shutdown();
			}
		}

	private:
		bool initialized_ = false;
	};
}

#ifndef DEV_BUILD
REGISTER_COMPONENT(discord::component)
#endif
REGISTER_COMPONENT(discord::component)
