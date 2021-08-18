#pragma once
#include "FunctionFwd.hpp"

enum class AnimId
{
    Abe_Head_Gib,
    Abe_Arm_Gib,
    Abe_Body_Gib,
    BlindMud_Head_Gib,
    BlindMud_Arm_Gib,
    BlindMud_Body_Gib,
    Glukkon_Head_Gib,
    Glukkon_Arm_Gib,
    Glukkon_Body_Gib,
    Aslik_Head_Gib,
    Aslik_Arm_Gib,
    Aslik_Body_Gib,
    Dripik_Head_Gib,
    Dripik_Arm_Gib,
    Dripik_Body_Gib,
    Phleg_Head_Gib,
    Phleg_Arm_Gib,
    Phleg_Body_Gib,
    Fleech_Head_Gib,
    Fleech_Body_Gib,
    Meat_Gib,
    Metal_Gib,
    Rock_Gib,
    Slig_Head_Gib,
    Slig_Arm_Gib,
    Slig_Body_Gib,
    Slog_Head_Gib,
    Slog_Body_Gib,
    Stick_Gib,

    Abe_Crouch_Start,
    Abe_Crouch_Talk_A,
    Abe_Crouch_Talk_B,
    Abe_Death_Fall,
    Abe_Death_Gas,
    Abe_Death_Z_A,
    Abe_Death_Z_B,
    Abe_Door_Enter,
    Abe_Door_Exit,
    Abe_Fall_A,
    Abe_Fall_B,
    Abe_Fall_C,
    Abe_Handstone_End,
    Abe_Handstone_Short,
    Abe_Handstone_Start,
    Abe_Hoist,
    Abe_Hoist_Down,
    Abe_Hoist_High,
    Abe_Hoist_Swing,
    Abe_Hoist_Up,
    Abe_Hop,
    Abe_Hop_End,
    Abe_Hop_Start,
    Abe_Item_Grab,
    Abe_Knock_Back,
    Abe_Knock_Forward,
    Abe_Lift,
    Abe_Lift_Down,
    Abe_Lift_End,
    Abe_Lift_Start,
    Abe_Lift_Up,
    Abe_Mine_Car_Enter,
    Abe_Mine_Car_Exit,
    Abe_Push,
    Abe_Rock,
    Abe_Rock_Cancel,
    Abe_Rock_Crouch,
    Abe_Rock_Crouch_Throw,
    Abe_Rock_Throw,
    Abe_Roll,
    Abe_Roll_End,
    Abe_Roll_Start,
    Abe_Shrug_End,
    Abe_Shrykull_End,
    Abe_Shrykull_Start,
    Abe_Slap_Bomb,
    Abe_Sorry_Start,
    Abe_Sorry_End,
    Abe_Unknown_A,
    Abe_Unknown_B,
    Abe_Unknown_C,
    Abe_Unknown_D,
    Abe_Unknown_E,
    Abe_Unknown_F,
    Abe_Unknown_G,
    Abe_Unknown_H,
    Abe_Unknown_I,
    Abe_Unknown_J,
    Abe_Well,
    Abe_Well_Enter,
    Abe_Well_Enter_Start,

    Bat,
    Bee_Swarm,

    Fleech_Climb,
    Fleech_SettleOnGround,
    Fleech_Consume,
    Fleech_Fall,
    Fleech_Sleeping,
    Fleech_Idle,
    Fleech_StopCrawling,
    Fleech_StopMidCrawlCycle,
    Fleech_Land,
    Fleech_SleepingWithTongue,
    Fleech_PatrolCry,
    Fleech_RaiseHead,
    Fleech_WakingUp,
    Fleech_Unused,
    Fleech_Knockback,
    Fleech_ExtendTongueFromEnemy,
    Fleech_RetractTongueFromEnemey,
    Fleech_DeathByFalling,
    Fleech_Crawl,

    Flying_Slig_Idle,
    Flying_Slig_Move_Horizontal,
    Flying_Slig_Idle_Turn_Around,
    Flying_Slig_Move_Down,
    Flying_Slig_Move_Down_Turn_Around,
    Flying_Slig_Move_Up,
    Flying_Slig_Move_Up_Turn_Around,
    Flying_Slig_Pull_Lever,
    Flying_Slig_Speak,
    Flying_Slig_Possession,
    Flying_Slig_Move_Horizontal_End,
    Flying_Slig_Move_Up_Start,
    Flying_Slig_Move_Horizontal_To_Down,
    Flying_Slig_Move_Up_To_Horizontal,
    Flying_Slig_Move_Down_To_Horizontal,
    Flying_Slig_Turn_Quick,
    Flying_Slig_Idle_To_Horizontal,
    Flying_Slig_Move_Down_Start,
    Flying_Slig_Move_Down_End,
    Flying_Slig_Knockback_Down,
    Flying_Slig_Knockback_Up,
    Flying_Slig_Move_Up_End,
    Flying_Slig_Up_Turn_Instant,
    Flying_Slig_Down_Turn_Instant,
    Flying_Slig_Move_Horizontal_To_Up,
    Flying_Slig_Turn_Horizontal,

    Glukkon_Aslik_KnockBack,
    Glukkon_Aslik_Idle,
    Glukkon_Aslik_Jump,
    Glukkon_Aslik_Land,
    Glukkon_Aslik_LongLaugh,
    Glukkon_Aslik_ChantShake,
    Glukkon_Aslik_GetShot,
    Glukkon_Aslik_Speak1,
    Glukkon_Aslik_Speak2,
    Glukkon_Aslik_Speak3,
    Glukkon_Aslik_KnockBackStandBegin,
    Glukkon_Aslik_KnockBackStandEnd,
    Glukkon_Aslik_Turn,
    Glukkon_Aslik_JumpToFall,
    Glukkon_Aslik_WalkToFall,
    Glukkon_Aslik_Fall,
    Glukkon_Aslik_DeathFall,
    Glukkon_Aslik_BeginWalk,
    Glukkon_Aslik_EndWalk,
    Glukkon_Aslik_StandToJump,
    Glukkon_Aslik_JumpToStand,
    Glukkon_Aslik_WalkToJump,
    Glukkon_Aslik_JumpToWalk,
    Glukkon_Aslik_EndSingleStep,
    Glukkon_Aslik_Walk,

    Glukkon_Dripik_KnockBack,
    Glukkon_Dripik_Idle,
    Glukkon_Dripik_Jump,
    Glukkon_Dripik_Land,
    Glukkon_Dripik_LongLaugh,
    Glukkon_Dripik_ChantShake,
    Glukkon_Dripik_GetShot,
    Glukkon_Dripik_Speak1,
    Glukkon_Dripik_Speak2,
    Glukkon_Dripik_Speak3,
    Glukkon_Dripik_KnockBackStandBegin,
    Glukkon_Dripik_KnockBackStandEnd,
    Glukkon_Dripik_Turn,
    Glukkon_Dripik_JumpToFall,
    Glukkon_Dripik_WalkToFall,
    Glukkon_Dripik_Fall,
    Glukkon_Dripik_DeathFall,
    Glukkon_Dripik_BeginWalk,
    Glukkon_Dripik_EndWalk,
    Glukkon_Dripik_StandToJump,
    Glukkon_Dripik_JumpToStand,
    Glukkon_Dripik_WalkToJump,
    Glukkon_Dripik_JumpToWalk,
    Glukkon_Dripik_EndSingleStep,
    Glukkon_Dripik_Walk,

    Glukkon_Normal_KnockBack,
    Glukkon_Normal_Idle,
    Glukkon_Normal_Jump,
    Glukkon_Normal_Land,
    Glukkon_Normal_LongLaugh,
    Glukkon_Normal_ChantShake,
    Glukkon_Normal_GetShot,
    Glukkon_Normal_Speak1,
    Glukkon_Normal_Speak2,
    Glukkon_Normal_Speak3,
    Glukkon_Normal_KnockBackStandBegin,
    Glukkon_Normal_KnockBackStandEnd,
    Glukkon_Normal_Turn,
    Glukkon_Normal_JumpToFall,
    Glukkon_Normal_WalkToFall,
    Glukkon_Normal_Fall,
    Glukkon_Normal_DeathFall,
    Glukkon_Normal_BeginWalk,
    Glukkon_Normal_EndWalk,
    Glukkon_Normal_StandToJump,
    Glukkon_Normal_JumpToStand,
    Glukkon_Normal_WalkToJump,
    Glukkon_Normal_JumpToWalk,
    Glukkon_Normal_EndSingleStep,
    Glukkon_Normal_Walk,

    Glukkon_Phleg_KnockBack,
    Glukkon_Phleg_Idle,
    Glukkon_Phleg_Jump,
    Glukkon_Phleg_Land,
    Glukkon_Phleg_LongLaugh,
    Glukkon_Phleg_ChantShake,
    Glukkon_Phleg_GetShot,
    Glukkon_Phleg_Speak1,
    Glukkon_Phleg_Speak2,
    Glukkon_Phleg_Speak3,
    Glukkon_Phleg_KnockBackStandBegin,
    Glukkon_Phleg_KnockBackStandEnd,
    Glukkon_Phleg_Turn,
    Glukkon_Phleg_JumpToFall,
    Glukkon_Phleg_WalkToFall,
    Glukkon_Phleg_Fall,
    Glukkon_Phleg_DeathFall,
    Glukkon_Phleg_BeginWalk,
    Glukkon_Phleg_EndWalk,
    Glukkon_Phleg_StandToJump,
    Glukkon_Phleg_JumpToStand,
    Glukkon_Phleg_WalkToJump,
    Glukkon_Phleg_JumpToWalk,
    Glukkon_Phleg_EndSingleStep,
    Glukkon_Phleg_Walk,

    Greeter_Chase,
    Greeter_Falling,
    Greeter_Hit,
    Greeter_Idle,
    Greeter_Speak,
    Greeter_Turn_Around,

    Mine_Car_Closed,
    Mine_Car_Open,
    Mine_Car_Shake_A,
    Mine_Car_Shake_B,
    Mine_Car_Tread_Idle,
    Mine_Car_Tread_Move_A,
    Mine_Car_Tread_Move_B,

    Mudokon_Chant,
    Mudokon_Chant_End,
    Mudokon_Chisel_Mining,
    Mudokon_Crouch,
    Mudokon_Crouch_To_Duck,
    Mudokon_Crouch_To_Stand,
    Mudokon_Crouch_Turn_Around,
    Mudokon_Duck,
    Mudokon_Duck_To_Crouch,
    Mudokon_Fall,
    Mudokon_Get_Up,
    Mudokon_Hit_Self,
    Mudokon_Hoist,
    Mudokon_Hoist_End,
    Mudokon_Hoist_Start,
    Mudokon_Idle,
    Mudokon_Knocked_Back_Face_Down,
    Mudokon_Knocked_Back_Face_Up,
    Mudokon_Lever_Pull,
    Mudokon_Run,
    Mudokon_Run_End_A,
    Mudokon_Run_End_B,
    Mudokon_Run_End_C,
    Mudokon_Run_Jump,
    Mudokon_Run_Jump_Start,
    Mudokon_Scrub_Cleaning,
    Mudokon_Shrug_Start,
    Mudokon_Shrug_End,
    Mudokon_Slap,
    Mudokon_Sling,
    Mudokon_Sneak,
    Mudokon_Sneak_End,
    Mudokon_Sneak_Start,
    Mudokon_Stand_To_Crouch,
    Mudokon_Talk_A,
    Mudokon_Talk_B,
    Mudokon_Talk_C,
    Mudokon_Talk_D,
    Mudokon_Turn_Around,
    Mudokon_Unknown_A,
    Mudokon_Unknown_B,
    Mudokon_Unknown_C,
    Mudokon_Unknown_E,
    Mudokon_Unknown_F,
    Mudokon_Unknown_G,
    Mudokon_Unknown_H,
    Mudokon_Unknown_I,
    Mudokon_Unknown_J,
    Mudokon_Unknown_K,
    Mudokon_Unknown_L,
    Mudokon_Unknown_M,
    Mudokon_Unknown_N,
    Mudokon_Unknown_O,
    Mudokon_Unknown_P,
    Mudokon_Unknown_Q,
    Mudokon_Unknown_R,
    Mudokon_Unknown_S,
    Mudokon_Unknown_T,
    Mudokon_Walk,
    Mudokon_Wheel,
    Mudokon_Wheel_End,
    Mudokon_Wheel_Start,

    Paramite_Attack,
    Paramite_WebGoingDown,
    Paramite_WebLeaveDown,
    Paramite_WebLeaveUp,
    Paramite_WebIdle,
    Paramite_WebGrab,
    Paramite_WebGoingUp,
    Paramite_Eating,
    Paramite_Death,
    Paramite_Idle,
    Paramite_WalkBegin,
    Paramite_JumpUpMidair,
    Paramite_JumpUpLand,
    Paramite_JumpUpBegin,
    Paramite_Walking,
    Paramite_RunningAttack,
    Paramite_Running,
    Paramite_Hop,
    Paramite_PreHiss,
    Paramite_Squawk,
    Paramite_AllOYaGameSpeakBegin,
    Paramite_GameSpeakEnd,
    Paramite_GameSpeakBegin,
    Paramite_Turn,
    Paramite_Unused,
    Paramite_WalkRunTransition,
    Paramite_WalkEnd,
    Paramite_RunBegin,
    Paramite_RunEnd,
    Paramite_Falling,
    Paramite_RopePull,
    Paramite_CloseAttack,
    Paramite_Landing,
    Paramite_Unused2,
    Paramite_Knockback,
    Paramite_SurpriseWeb,
    Paramite_Hiss,
    Paramite_PostHiss,

    Scrab_FeedToGulp,
    Scrab_Dance,
    Scrab_DeathEnd,
    Scrab_Feed,
    Scrab_StandToFeed,
    Scrab_Idle,
    Scrab_Jump,
    Scrab_LegKick,
    Scrab_HopBegin,
    Scrab_Landing,
    Scrab_Knockback,
    Scrab_Shriek,
    Scrab_Run,
    Scrab_RunToStand,
    Scrab_HowlBegin,
    Scrab_HowlEnd,
    Scrab_Turn,
    Scrab_JumpAndRunToFall,
    Scrab_StandToWalk,
    Scrab_StandToRun,
    Scrab_WalkToStand,
    Scrab_WalkToFall,
    Scrab_WalkToRun,
    Scrab_RunToWalk,
    Scrab_GetEaten,
    Scrab_Empty,
    Scrab_GulpToStand,
    Scrab_AttackLunge,
    Scrab_DeathBegin,
    Scrab_Walk,
    Scrab_AttackSpin,
    Scrab_ScrabBattleAnim,

    Crawling_Slig_Idle,
    Crawling_Slig_UsingButton,
    Crawling_Slig_WakingUp,
    Crawling_Slig_Crawling,
    Crawling_Slig_StartFalling,
    Crawling_Slig_Falling,
    Crawling_Slig_Landing,
    Crawling_Slig_ToShakingToIdle,
    Crawling_Slig_Speaking,
    Crawling_Slig_Snoozing,
    Crawling_Slig_PushingWall,
    Crawling_Slig_TurnAround,
    Crawling_Slig_Shaking,
    Crawling_Slig_Empty,
    Crawling_Slig_ShakingToIdle,
    Crawling_Slig_EndCrawling,
    Crawling_Slig_IdleToPushingWall,
    Crawling_Slig_EndPushingWall,

    Slig_Beat,
    Slig_OutToFall,
    Slig_LandingFatal,
    Slig_Smash,
    Slig_LandingSoft,
    Slig_FallingInitiate,
    Slig_KnockbackToStand,
    Slig_Idle,
    Slig_ShootToStand,
    Slig_Knockeback,
    Slig_PullLever,
    Slig_LiftGripping,
    Slig_LiftUp,
    Slig_LiftDown,
    Slig_LiftGrip,
    Slig_LiftUngrip,
    Slig_PossessShake,
    Slig_ReloadGun,
    Slig_Running,
    Slig_StandToRun,
    Slig_SlidingToStand,
    Slig_SlidingTurn,
    Slig_Shoot,
    Slig_Recoil,
    Slig_ShootZ,
    Slig_ShootZtoStand,
    Slig_SteppingToStand,
    Slig_Sleeping,
    Slig_Speak1,
    Slig_Speak2,
    Slig_Speak3,
    Slig_Speak4,
    Slig_TurnAroundStanding,
    Slig_Walking,
    Slig_StandToWalk,
    Slig_SlidingTurnToWalk,
    Slig_SlidingTurnToRun,
    Slig_StandingToStep,
    Slig_Gamespeak,
    Slig_WalkToStand,
    Slig_SleepingToStand,

    Slog_MoveHeadUpwards,
    Slog_StartFastBarking,
    Slog_AngryBark,
    Slog_BROKEN_J,
    Slog_Dying,
    Slog_Eating,
    Slog_Fall,
    Slog_Idle,
    Slog_Growl,
    Slog_JumpUpwards,
    Slog_JumpForwards,
    Slog_Run,
    Slog_StopRunning,
    Slog_SlideTurn,
    Slog_Scratch,
    Slog_Sleeping,
    Slog_MoveHeadDownwards,
    Slog_Bark,
    Slog_TurnAround,
    Slog_StartWalking,
    Slog_EndWalking,
    Slog_Land,
    Slog_Unused,
    Slog_EndFastBarking,
    Slog_Walk,

    Slurg_Burst,
    Slurg_Move,
    Slurg_Turn_Around,

    BG_Brewery_Barrel_Anim,
    BG_Mine_Fan,
    BG_Feeco_Small_Fan,
    BG_Heat_Extractor_Fan,
    
    Background_Glukkon,
    Bell_Hammer,
    Bird_Portal_Orb,
    Blood,
    Bone,
    Bone_Bag_A,
    Bone_Bag_B,
    Bone_Bag_C,
    Bone_Saw_Horizontal_Off,
    Bone_Saw_Horizontal_On,
    Bone_Saw_Vertical_Off,
    Bone_Saw_Vertical_On,
    Bullet_Casing,
    Chime_A,
    Chime_B,
    Chime_C,
    Chime_Ball,
    Circular_Fade,
    Death_Bird_Flare,
    Death_Flare,
    Door_Barracks_Closed,
    Door_Barracks_Closing,
    Door_Bonewerkz_Closed,
    Door_Bonewerkz_Closing,
    Door_Brewery_Closed,
    Door_Brewery_Closing,
    Door_Feeco_Closed,
    Door_Feeco_Closing,
    Door_Flame,
    Door_Flame_Glow,
    Door_Flame_Spark,
    Door_Lock_Idle,
    Door_Lock_Open,
    Door_Mines_Closed,
    Door_Mines_Closing,
    Door_Temple_Closed,
    Door_Temple_Closing,
    Door_Train_Closed,
    Door_Train_Closing,
    Dove_Idle,
    Dove_Flying,
    Electric_Wall,
    Explosion,
    Explosion_Mine,
    Explosion_Small,
    Falling_Crate_A,
    Falling_Crate_B,
    Falling_Rock_A,
    Falling_Rock_B,
    Fart,
    Foot_Switch_Bonewerkz_Idle,
    Foot_Switch_Bonewerkz_Pressed,
    Foot_Switch_Industrial_Idle,
    Foot_Switch_Industrial_Pressed,
    Foot_Switch_Temple,
    Foot_Switch_Vault_Idle,
    Foot_Switch_Vault_Pressed,
    Grenade,
    Grenade_Machine_Button_Off,
    Grenade_Machine_Button_On,
    Grenade_Machine_Nozzle,
    Grenade_Machine_Nozzle_Drop_Grenade,
    Hint_Fly,
    Honey,
    Honey_Drip,
    Honey_Hive,
    Lift_Rope,
    Meat,
    Meat_Sack,
    Meat_Saw,
    Mine,
    Motion_Detector_Flare,
    Motion_Detector_Laser,
    Moving_Bomb,
    Paramite_Web,
    Pullring_Desert,
    Pullring_Farms,
    Rock,
    Rock_Sack_A,
    Rock_Sack_B,
    Rock_Sack_Hit,
    Rope_A,
    Rope_B,
    Security_Claw_Lower,
    Security_Claw_Upper,
    Security_Door,
    Security_Door_Idle,
    Security_Door_Speak,
    Security_Orb,
    Slam_Door_Industrial_Closed,
    Slam_Door_Industrial_Closing,
    Slam_Door_Industrial_Opening,
    Slam_Door_Vault_Closed,
    Slam_Door_Vault_Closing,
    Slam_Door_Vault_Opening,
    Slap_Lock_Idle_A,
    Slap_Lock_Idle_B,
    Slap_Lock_Shake,
    Sparks,
    Star,
    Status_Light_Green,
    Status_Light_Red,
    Stone_Ball,
    Stone_Ball_Stopper,
    Swinging_Ball_Fast,
    Swinging_Ball_Normal,
    Swinging_Ball_Slow,
    Switch_Idle,
    Switch_Pull_Left_A,
    Switch_Pull_Left_B,
    Switch_Pull_Right_A,
    Switch_Pull_Right_B,
    Timed_Mine_Armed,
    Timed_Mine_Idle,
    Tortured_Mudokon,
    Tortured_Mudokon_Released,
    Tortured_Mudokon_Zap,
    Trap_Door_Closed,
    Trap_Door_Closing,
    Trap_Door_Open,
    Trap_Door_Opening,
    Trap_Door_Tribal_Closed,
    Trap_Door_Tribal_Closing,
    Trap_Door_Tribal_Open,
    Trap_Door_Tribal_Opening,
    R1_TrapDoor_Open,
    R1_TrapDoor_Closed,
    R1_TrapDoor_Opening,
    R1_TrapDoor_Closing,
    Lines_TrapDoor_Open,
    Lines_TrapDoor_Closed,
    Lines_TrapDoor_Opening,
    Lines_TrapDoor_Closing,
    Desert_TrapDoor_Open,
    Desert_TrapDoor_Closed,
    Desert_TrapDoor_Opening,
    Desert_TrapDoor_Closing,
    UXB_Active,
    UXB_Disabled,
    UXB_Toggle,
    Well_Leaf,
    Work_Wheel_Idle,
    Work_Wheel_Turning,
    Zap_Line_Blue,
    Zap_Line_Red,
    Zap_Sparks,

    Anim_Tester, // For animation testing with the TestAnimation.cpp, ignore this.
};

enum class PalId
{
    Default, // Don't override the anims default pal
    BlindMud,
};

struct PalDetails final
{
    const char_type* mBanName;
    s32 mResourceId;
};

struct CombinedPalRecord final
{
    PalId mId;
    PalDetails mAEData;
    PalDetails mAOData;
};

struct PalRecord final
{
    PalId mId;
    const char_type* mBanName;
    s32 mResourceId;
};

struct AnimDetails final
{
    const char_type* mBanName;
    s32 mFrameTableOffset;
    s16 mMaxW;
    s16 mMaxH;
    s32 mResourceId;
    PalId mPalOverride;
};

struct AnimRecord final
{
    AnimId mId;
    const char_type* mBanName;
    s32 mFrameTableOffset;
    s16 mMaxW;
    s16 mMaxH;
    s32 mResourceId;
    PalId mPalOverride;
};

struct CombinedAnimRecord final
{
    AnimId mId;
    AnimDetails mAEData;
    AnimDetails mAOData;
};

const PalRecord PalRec(PalId toFind);
const AnimRecord AnimRec(AnimId toFind);

namespace AO 
{
    const PalRecord PalRec(PalId toFind);
    const AnimRecord AnimRec(AnimId toFind);
}

// AE and AO resource ids
enum ResourceID
{
    kUnknownResID_0 = 0,
    kDemoResID = 1,
    kLcdfontResID = 2,
    kAbebasicResID = 10,
    kAbepullResID = 11,
    kAbepickResID = 12,
    kAbebombResID = 13,
    kAbethrowResID = 14,
    kAbewaspResID = 16,
    kAberingResID = 17,
    kAbesmashResID = 19,
    kAbefallResID = 20,
    kAbestoneResID = 21,
    kAbecrushResID = 22,
    kAbeblowResID = 25,
    kAbeknbkResID = 26,
    kAbeknfdResID = 27,
    kAbeknokzResID = 28,
    kAbeyawnResID = 31,
    kAberubResID = 32,
    kAbesizeResID = 33,
    kAbebordResID = 34,
    kAbeeyesResID = 35,
    kAbebuttResID = 36,
    kAbenutsResID = 37,
    kAbepnlkResID = 38,
    kAbewizzResID = 39,
    kAbetalkResID = 40,
    kAbehoistResID = 42,
    kAbeedgeResID = 43,
    kAbeswingResID = 44,
    kAbedoorResID = 45,
    kAbewellResID = 47,
    kAbeommResID = 48,
    kAbeliftResID = 53,
    kAbebsic1ResID = 55,
    kDovbasicResID = 60,
    kWaspResID = 61,
    kHintflyResID = 62,
    kBatBasicResID = 63,
    kFireFlyResID = 64,
    kDovshootResID = 66,
    kElumUnknownResID_100 = 100, // AO only
    kAneknbkResID = 106,         // AO only
    kAneedgeResID = 108,         // AO only
    kElumUnknownResID_110 = 110, // AO only
    kElumUnknownResID_112 = 112, // AO only
    kAbeCarResId = 113,
    kAneprmntResID = 115,
    kAbemorphResID = 117,
    kAbegasResID = 118,
    kShrmorphResID = 121,
    kUnknownResID_125 = 125,
    kAbespeakResID = 130,
    kAbeintroResID = 131,
    kAbespek2ResID = 132,
    kSligSpeakResID = 140,
    kGlukkonSpeakResID = 150,
    kParamiteSpeakResID = 160,
    kScrabSpeakResID = 170,
    kElmbasicResID_200 = 200, // AO only
    kHappyiconResID = 201,
    kAngryiconResID = 202,
    kNormaliconResID = 203,
    kUnknownResID_204 = 204,
    kElmnknbkResID = 215,    // AO only
    kElmfallResID_216 = 216, // AO only
    kElmblowResID_217 = 217,
    kElumUnknownResID_220 = 220, // AO only
    kElumUnknownResID_221 = 221, // AO only
    kElmprmntResID__222 = 222,   // AO only
    kElumUnknownResID_223 = 223, // AO only
    kElumUnknownResID_224 = 224, // AO only
    kElmaloneResID_230 = 230,    // AO only
    kBgexpldResID = 300,
    kExplo2ResID = 301,
    kAlrmFlarResID = 302,
    kDustResID = 303,
    kHubFireResID = 304,
    kVaporResID = 305,
    kSlurgResID = 306,
    kGreeterResID = 307,
    kOmmflareResID = 312,
    kPortalTerminatorID = 313,
    kSparksResID = 314,
    kSpotliteResID = 316,
    kSkiddustResID = 317,
    kSmokeResID = 318,
    kBigflashResID = 319,
    kWorkwhelResID = 320,
    kUnknownResID_333 = 333,
    kHoneyResID = 337,
    kWellLeafResID = 341,
    kSquibResID = 343,
    kZflashResID = 344,
    kFxflareResID = 348,
    kDeathFlareResID = 349,
    kAberockResID = 350,
    kPortliteResID = 351,
    kPortlitResID = 353,
    kSquibSmokeResID = 354,
    kSplineResID = 355,
    kHoistRocks = 357,
    kStickGib = 358,
    kShellResID = 360,
    kGlowResID = 361,
    kMetalGib = 365,
    kBloodropResID = 366,
    kOptionFlare = 367,
    kWaterDrop = 368,
    kSplashResID = 369,
    kSmallExplo2ResID = 372,
    kStatusLightResID = 373,
    kDoorLockResID_374 = 374,
    kTorturedMudTearsResID = 375,
    kSlgbasicResID = 412,
    kSlgsleepResID = 413,
    kSlgknbkResID = 414,
    kSlgedgeResID = 415,
    kSlgsmashResID = 416,
    kSlgzshotResID = 417,
    kSlgknfdResID = 418,
    kSlgleverResID = 419,
    kSlgliftResID = 420,
    kSligBlowResID = 423,
    kSlgbeatResID = 426,
    kUnknownResID_448 = 448,
    kCrawlingSligResID_449 = 449,
    kFlySligResID = 450,
    kSlingmudResID_508 = 508,
    kMudscrubResID = 510,
    kMudchslResID = 511,
    kMudidleResID = 512,
    kMudoduckResID = 514,
    kAbeworkResID = 515,
    kMudltusResID = 516,
    kMudbtlnkResID = 517,
    kTorturedMud = 518,
    kMudangryResID = 530,
    kMudsadResID = 531,
    kMudwiredResID = 532,
    kMudblindResID = 533,
    kMudsickResID = 534,
    kMudombieResID = 550,
    kDogbasicResID = 570,
    kDogrstnResID = 571,
    kDogattkResID = 572,
    kDogknfdResID = 573,
    kDogidleResID = 574,
    kSlogBlowResID = 576,
    kFleeBlowResID_580 = 580,
    kBoneBagResID_590 = 590,
    kBoneResID = 591,
    kArjbasicResID = 600,
    kArjeatResID = 601,
    kArjclimbResID = 604,
    kArjpumpResID = 605,
    kArjscrchResID = 609,
    kArjwaspResID = 610,
    kArjjumpResID = 611,
    kArjponceResID = 614,
    kArjfalrkResID = 615,
    kArsbasicResID = 700,
    kArseatResID = 701,
    kArsprceResID = 702,
    kArsskwrResID = 703,
    kArswhirlResID = 704,
    kArshowlResID = 705,
    kArsdanceResID = 706,
    kArssackResID = 707,
    kArsdeadResID = 708,
    kArsroarResID = 709,
    kArsgrwlResID = 710,
    kArschewResID = 711,
    kArscrshResID = 713,
    kGlkbasicResID = 800,
    kGlukkonResID_801 = 801,
    kGlukAslikResID = 802,
    kAslikResID_803 = 803,
    kGlukDripikResID = 804,
    kDripikResID_805 = 805,
    kGlukPhlegResID = 806,
    kPhlegResID_807 = 807,
    kGlukredResID = 825,
    kGlukgrenResID = 826,
    kGlukblueResID = 827,
    kGlukaquaResID = 828,
    kFleechResID = 900,
    kRopesResID = 1000,
    kPulleyResID = 1001,
    kP2c2bagResID = 1002,
    kP5c1wlkResID = 1003,
    kP6c1trapResID = 1004,
    kBombResID = 1005,
    kGrenflshResID = 1006,
    kP4c5hiveResID = 1008,
    kSwitchResID = 1009,
    kBombflshResID = 1011,
    kP2c3HiveResID = 1013,
    kPullringResID = 1014,
    kP6c4tubeResID = 1017,
    kP7c1spikResID = 1018,
    kHammerResID = 1019,
    kCrystalResID = 1030,
    kLiftResID = 1032,
    kLandmineResID = 1036,
    kTbombResID = 1037,
    kBGStarResID = 1039,
    kMineflshResID = 1040,
    kP1c3tubeResID = 1050,
    kGhostTrpResID_1053 = 1053,
    kUnknwonResID_1057 = 1057,
    kDebrisID00 = 1105,
    kTubeResID = 1200,

    kTestanimResID = 2000,

    kF2zballResID = 2001,
    kF2stnbalResID = 2002,
    kF2stnsckResID = 2003,
    kChimeResID = 2005,
    kMaimGameResID = 2006,
    kF2rockResID = 2007,
    kF2eyeorbResID = 2008,
    kPupilResID = 2009,
    kTriggerResID = 2010,
    kF2p3dorResID = 2012,
    kDoorResID_2013 = 2013,
    kFlintLockResID = 2017,
    kRockdoorResID = 2018,
    kHubdoorResID = 2019,
    kSlamResID = 2020,
    kMeatResID = 2023,
    kF2ChimeBallResID = 2024,
    kF2ChimeRackResID = 2025,
    kGourdResID = 2028,
    kWebResID = 2034,
    kRockShadowResID = 2035,
    kMovebombResID = 3006,
    kD2elumResID = 4002,
    kElecwallResID = 6000,
    kMotionResID = 6001,
    kMflareResID = 6002,
    kMeatsawResID = 6003,
    kDrillResID = 6004,
    kGrenadeResID = 6005,
    kR1sdoorResID = 6006,
    kR1buttonResID = 6008,
    kR1bpipeResID = 6009,
    kGreenGlowResID_6010 = 6010, // AO only
    kRedGlowResID_6011 = 6011,   // AO only
    kBayrollResID_6013 = 6013,
    kMeatGibResID_6014 = 6014, // AO only
    kUnknownResID_6016 = 6016,
    kEvilFartResID = 6017,
    kR1sdosResID_6027 = 6027,
    kFlintGlowResID_6028 = 6028,      // AO only
    kGreenDoorLightResID_6031 = 6031, // AO only
    kRedDoorLightResID_6032 = 6032,   // AO only
    kTwnkleResID = 7012,
    kControlsResID = 8000,
    kDoorResID = 8001,
    kHighliteResID = 8002,
    kHighlitePalResID = 8003,
    kLoadingResID = 8010,
};

namespace AO {

enum AOResourceID
{
};

} // namespace AO
