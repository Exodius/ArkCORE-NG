/*
 * Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2011-2016 ArkCORE <http://www.arkania.net/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Player.h"
#include "SpellInfo.h"

// 2530
class npc_yenniku_2530 : public CreatureScript
{
public:
    npc_yenniku_2530() : CreatureScript("npc_yenniku") { }

    struct npc_yenniku_2530AI : public ScriptedAI
    {
        npc_yenniku_2530AI(Creature* creature) : ScriptedAI(creature)
        {
            bReset = false;
        }

        uint32 Reset_Timer;
        bool bReset;

        void Reset() override
        {
            Reset_Timer = 0;
            me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_NONE);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell) override
        {
            if (bReset || spell->Id != 3607)
                return;

            if (Player* player = caster->ToPlayer())
            {
                if (player->GetQuestStatus(592) == QUEST_STATUS_INCOMPLETE) //Yenniku's Release
                {
                    me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_STUN);
                    me->CombatStop();                   //stop combat
                    me->DeleteThreatList();             //unsure of this
                    me->setFaction(83);                 //horde generic

                    bReset = true;
                    Reset_Timer = 60000;
                }
            }
        }

        void EnterCombat(Unit* /*who*/) override { }

        void UpdateAI(uint32 diff) override
        {
            if (bReset)
            {
                if (Reset_Timer <= diff)
                {
                    EnterEvadeMode();
                    bReset = false;
                    me->setFaction(28);                     //troll, bloodscalp
                    return;
                }

                Reset_Timer -= diff;

                if (me->IsInCombat() && me->GetVictim())
                {
                    if (Player* player = me->EnsureVictim()->ToPlayer())
                    {
                        if (player->GetTeam() == HORDE)
                        {
                            me->CombatStop();
                            me->DeleteThreatList();
                        }
                    }
                }
             }

            //Return since we have no target
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_yenniku_2530AI(creature);
    }
};

// 43505 bossy
class npc_bossy_43505 : public CreatureScript
{
public:
    npc_bossy_43505() : CreatureScript("npc_bossy_43505") { }

    enum eQuest
    {
        QUEST_LOOKS_LIKE_A_TAUREN_PIRATE_TO_ME = 26630,
        ITEM_COW_HEAD = 59147,
        SPELL_BOSSYS_SACRIFICE = 81311,
    };

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (player->GetQuestStatus(QUEST_LOOKS_LIKE_A_TAUREN_PIRATE_TO_ME) == QUEST_STATUS_INCOMPLETE)
            if (player->GetItemCount(ITEM_COW_HEAD) == 0)
            {
                player->ADD_GOSSIP_ITEM_DB(11741, 0, GOSSIP_SENDER_MAIN, 1001);
                player->ADD_GOSSIP_ITEM_DB(11741, 1, GOSSIP_SENDER_MAIN, 1002);
                player->SEND_GOSSIP_MENU(RAND(16437, 16438), creature->GetGUID());
            }
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    { 
        if (player->GetQuestStatus(QUEST_LOOKS_LIKE_A_TAUREN_PIRATE_TO_ME) == QUEST_STATUS_INCOMPLETE)
            if (player->GetItemCount(ITEM_COW_HEAD) == 0)
            {
                switch (action)
                {
                    case 1001:
                    {
                        player->PlayerTalkClass->ClearMenus();
                        player->ADD_GOSSIP_ITEM_DB(11742, 0, GOSSIP_SENDER_MAIN, 1003);
                        player->ADD_GOSSIP_ITEM_DB(11742, 1, GOSSIP_SENDER_MAIN, 1004);
                        player->SEND_GOSSIP_MENU(16439, creature->GetGUID());
                        break;
                    }
                    case 1002:
                    {
                        // say muuhhh ???
                        player->PlayerTalkClass->SendCloseGossip();
                        break;
                    }
                    case 1003:
                    {
                        // kill
                        creature->CastSpell(creature, SPELL_BOSSYS_SACRIFICE, true);
                        player->AddItem(ITEM_COW_HEAD, 1);
                        player->PlayerTalkClass->SendCloseGossip();
                        break;
                    }
                    case 1004:
                    {
                        // exit
                        player->PlayerTalkClass->SendCloseGossip();
                        break;
                    }
                }
                return true;
            }

        return false; 
    }

};

// 2663
class npc_narkk_2663 : public CreatureScript
{
public:
    npc_narkk_2663() : CreatureScript("npc_narkk_2663") { }

    enum eQuest
    {
        QUEST_LOOKS_LIKE_A_TAUREN_PIRATE_TO_ME = 26630,
        ITEM_OVERSIZED_PIRATE_HAT = 59148,
    };

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (player->GetQuestStatus(QUEST_LOOKS_LIKE_A_TAUREN_PIRATE_TO_ME) == QUEST_STATUS_INCOMPLETE)
            if (player->GetItemCount(ITEM_OVERSIZED_PIRATE_HAT) == 0)
            {
                player->ADD_GOSSIP_ITEM_DB(11743, 0, GOSSIP_SENDER_MAIN, 1001);
                player->ADD_GOSSIP_ITEM_DB(50045, 0, GOSSIP_SENDER_MAIN, 1002);
                player->SEND_GOSSIP_MENU(16440, creature->GetGUID());
                return true;
            }

        return false;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        if (player->GetQuestStatus(QUEST_LOOKS_LIKE_A_TAUREN_PIRATE_TO_ME) == QUEST_STATUS_INCOMPLETE)
            if (player->GetItemCount(ITEM_OVERSIZED_PIRATE_HAT) == 0)
            {
                switch (action)
                {
                    case 1001:
                    {
                        player->PlayerTalkClass->SendCloseGossip();
                        creature->AI()->Talk(0);
                        player->AddItem(ITEM_OVERSIZED_PIRATE_HAT, 1);
                        break;
                    }
                    case 1002:
                    {
                        player->GetSession()->SendListInventory(creature->GetGUID());
                        break;
                    }
                }
            }

        return false;
    }

};

// 2663
class npc_fleet_master_seahorn_2487 : public CreatureScript
{
public:
    npc_fleet_master_seahorn_2487() : CreatureScript("npc_fleet_master_seahorn_2487") { }

    enum eQuest
    {
        QUEST_LOOKS_LIKE_A_TAUREN_PIRATE_TO_ME = 26630,
        ITEM_COW_HEAD = 59147,
        ITEM_OVERSIZED_PIRATE_HAT = 59148,
        NPC_SEAHORNS_HEAD = 43506,
        NPC_BARON_REVILGAZ = 2496,
        
        ACTION_CAST_HEAD_VIDEO = 1,

        EVENT_CAST_HEAD_VIDEO = 1,
        EVENT_CAST_HEAD_BUG,

        playerGUID = 99999,
    };

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        player->PrepareQuestMenu(creature->GetGUID());
        if (player->GetQuestStatus(QUEST_LOOKS_LIKE_A_TAUREN_PIRATE_TO_ME) == QUEST_STATUS_INCOMPLETE)
            if (player->GetItemCount(ITEM_OVERSIZED_PIRATE_HAT) && player->GetItemCount(ITEM_COW_HEAD))
            {
                player->ADD_GOSSIP_ITEM_DB(50046, 0, GOSSIP_SENDER_MAIN, 1001);
                player->SEND_GOSSIP_MENU(16441, creature->GetGUID());
                return true;
            }

        return false;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        if (player->GetQuestStatus(QUEST_LOOKS_LIKE_A_TAUREN_PIRATE_TO_ME) == QUEST_STATUS_INCOMPLETE)
            if (player->GetItemCount(ITEM_OVERSIZED_PIRATE_HAT) && player->GetItemCount(ITEM_COW_HEAD))
            {
                switch (action)
                {
                    case 1001:
                    {
                        player->PlayerTalkClass->ClearMenus();
                        player->ADD_GOSSIP_ITEM_DB(11745, 0, GOSSIP_SENDER_MAIN, 1002);
                        player->SEND_GOSSIP_MENU(16442, creature->GetGUID());
                        break;
                    }
                    case 1002:
                    {
                        player->PlayerTalkClass->SendCloseGossip();
                        creature->AI()->Talk(0);
                        creature->GetAI()->DoAction(ACTION_CAST_HEAD_VIDEO);
                        creature->GetAI()->SetGUID(player->GetGUID(), playerGUID);
                        break;
                    }
                }
                return true;
            }

        return false;
    }

    bool OnQuestReward(Player* player, Creature* creature, Quest const* quest, uint32 /*opt*/) 
    { 
        if (player->GetQuestStatus(QUEST_LOOKS_LIKE_A_TAUREN_PIRATE_TO_ME) == QUEST_STATUS_INCOMPLETE)
            if (quest->GetQuestId() == QUEST_LOOKS_LIKE_A_TAUREN_PIRATE_TO_ME)
                creature->AI()->Talk(2);

        return false; 
    }

    struct npc_fleet_master_seahorn_2487AI : public ScriptedAI
    {
        npc_fleet_master_seahorn_2487AI(Creature* creature) : ScriptedAI(creature) { }

        EventMap m_events;
        uint64 m_playerGUID;
        uint64 m_baronGUID;
        uint64 m_headGUID;

        void Reset() override
        {
            m_events.Reset();
            m_playerGUID = 0;
            m_baronGUID = 0;
            m_headGUID = 0;
        }

        void DoAction(int32 param) 
        { 
            switch (param)
            {
                case ACTION_CAST_HEAD_VIDEO:
                {
                    m_events.ScheduleEvent(EVENT_CAST_HEAD_VIDEO, 25);
                    break;
                }
            }
        }

        void SetGUID(uint64 guid, int32 id) 
        { 
            switch (id)
            {
                case playerGUID:
                {
                    m_playerGUID = guid;
                    break;
                }
            }
        }

        void UpdateAI(uint32 diff) override
        {
            m_events.Update(diff);

            while (uint32 eventId = m_events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CAST_HEAD_VIDEO:
                    {
                        if (Creature* head = me->SummonCreature(NPC_SEAHORNS_HEAD, -14443.48f, 485.88f, 28.74f, 3.001957f, TEMPSUMMON_TIMED_DESPAWN, 15000))
                            m_headGUID = head->GetGUID();
                        if (Creature* npc = me->FindNearestCreature(NPC_BARON_REVILGAZ, 15.0f))
                        {
                            m_baronGUID = npc->GetGUID();
                            me->SetFacingToObject(npc);
                        }
                        m_events.ScheduleEvent(2, 3000);
                        break;
                    }
                    case 2:
                    {
                        if (Creature* head = sObjectAccessor->GetCreature(*me, m_headGUID))
                        {
                            me->HandleEmoteCommand(EMOTE_ONESHOT_KICK);
                            head->GetMotionMaster()->MoveJump(-14440.13f, 484.35f, 28.80f, 10.0f, 10.0f);
                        }
                        m_events.ScheduleEvent(3, 3000);
                        break;
                    }
                    case 3:
                    {
                        if (Creature* baron = sObjectAccessor->GetCreature(*me, m_baronGUID))
                            if (Creature* head = sObjectAccessor->GetCreature(*me, m_headGUID))
                            {
                                baron->HandleEmoteCommand(EMOTE_ONESHOT_KICK);
                                head->GetMotionMaster()->MoveJump(-14443.48f, 485.88f, 28.74f, 10.0f, 10.0f);
                            }
                        m_events.ScheduleEvent(4, 3000);
                        break;
                    }
                    case 4:
                    {
                        if (Creature* head = sObjectAccessor->GetCreature(*me, m_headGUID))
                        {
                            me->HandleEmoteCommand(EMOTE_ONESHOT_KICK);
                            head->GetMotionMaster()->MoveJump(-14440.13f, 484.35f, 28.80f, 10.0f, 10.0f);
                        }
                        m_events.ScheduleEvent(5, 3000);
                        break;
                    }
                    case 5:
                    {
                        if (Creature* baron = sObjectAccessor->GetCreature(*me, m_baronGUID))
                            if (Creature* head = sObjectAccessor->GetCreature(*me, m_headGUID))
                            {
                                baron->HandleEmoteCommand(EMOTE_ONESHOT_KICK);
                                head->GetMotionMaster()->MoveJump(-14443.48f, 485.88f, 28.74f, 10.0f, 10.0f);
                            }
                        m_events.ScheduleEvent(6, 3000);
                        break;
                    }
                    case 6:
                    {
                        Talk(1);
                        if (Creature* head = sObjectAccessor->GetCreature(*me, m_headGUID))
                            head->DespawnOrUnsummon();
                        if (Player* player = sObjectAccessor->GetPlayer(*me, m_playerGUID))
                        {
                            player->KilledMonsterCredit(2487);
                            player->CompleteQuest(QUEST_LOOKS_LIKE_A_TAUREN_PIRATE_TO_ME);
                        }
                        break;
                    }
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_fleet_master_seahorn_2487AI(creature);
    }
};

void AddSC_stranglethorn_vale()
{
    new npc_yenniku_2530();
    new npc_bossy_43505();
    new npc_narkk_2663();
    new npc_fleet_master_seahorn_2487();
}
