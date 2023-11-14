#pragma once

namespace relive {
struct SfxDefinition;
}

struct PathSoundInfo;
struct OpenSeqHandle;

namespace AO {

struct SoundBlockInfo;

enum class SeqId : s16
{
    None_M1 = -1,
    Unknown_0 = 0,
    Unknown_1 = 1,
    Unknown_2 = 2,
    Unknown_3 = 3,
    Unknown_4 = 4,
    Unknown_5 = 5,
    Unknown_6 = 6,
    Unknown_7 = 7,
    Unknown_8 = 8,
    Unknown_9 = 9,
    eHitBottomOfDeathPit_10 = 10,
    eMudokonChant1_11 = 11,
    eMudokonChant_12 = 12,
    eRingBellHammerAndExtraSound_13 = 13,
    Unknown_14 = 14,
    Unknown_15 = 15,
    Unknown_16 = 16,
    Unknown_17 = 17,
    eBatSqueaking_18 = 18,
    Unknown_19 = 19,
    Unknown_20 = 20,
    eExplosion1_21 = 21,
    Unknown_22 = 22,
    eAbeOops_23 = 23,
    Unknown_24 = 24,
    Unknown_25 = 25,
    Unknown_26 = 26,
    Unknown_27 = 27,
    Unknown_28 = 28,
    Unknown_29 = 29,
    eParamiteNearby_30 = 30,
    Unknown_31 = 31,
    Unknown_32 = 32,
    Unknown_33 = 33,
    Unknown_34 = 34,
    Unknown_35 = 35,
    Unknown_36 = 36,
    Unknown_37 = 37,
    Unknown_38 = 38,
    Unknown_39 = 39,
    Unknown_40 = 40,
    Unknown_41 = 41,
    Unknown_42 = 42,
    Unknown_43 = 43,
    Unknown_44 = 44,
    eSaveTriggerMusic_45 = 45,
    eSecretMusic_46 = 46,
    Unknown_47 = 47,
    Unknown_48 = 48,
    Unknown_49 = 49,
    Unknown_50 = 50,
    Unknown_51 = 51,
    Unknown_52 = 52,
    Unknown_53 = 53,
    Unknown_54 = 54,
    Unknown_55 = 55,
    Unknown_56 = 56,
    Unknown_57 = 57,
    Unknown_58 = 58,
    Unknown_59 = 59,
    Unknown_60 = 60,
    Unknown_61 = 61,
    Unknown_62 = 62,
    Unknown_63 = 63,
    Unknown_64 = 64,
    Unknown_65 = 65,
    Unknown_66 = 66,
    Unknown_67 = 67,
    Unknown_68 = 68,
    Unknown_69 = 69,
    Unknown_70 = 70,
    Unknown_71 = 71,
    Unknown_72 = 72,
    Unknown_73 = 73,
    Unknown_74 = 74,
    Unknown_75 = 75,
    Unknown_76 = 76,
    Unknown_77 = 77,
    Unknown_78 = 78,
    Unknown_79 = 79,
    Unknown_80 = 80,
    Unknown_81 = 81,
    Unknown_82 = 82,
    Unknown_83 = 83,
    Unknown_84 = 84,
    Unknown_85 = 85,
    Unknown_86 = 86,
    Unknown_87 = 87,
    Unknown_88 = 88,
    Unknown_89 = 89,
    Unknown_90 = 90,
    Unknown_91 = 91,
    Unknown_92 = 92,
    Unknown_93 = 93,
    Unknown_94 = 94,
    Unknown_95 = 95,
    Unknown_96 = 96,
    Unknown_97 = 97,
    Unknown_98 = 98,
    Unknown_99 = 99,
    Unknown_100 = 100,
    Unknown_101 = 101,
    Unknown_102 = 102,
    Unknown_103 = 103,
    Unknown_104 = 104,
    Unknown_105 = 105,
    Unknown_106 = 106,
    Unknown_107 = 107,
    Unknown_108 = 108,
    Unknown_109 = 109,
    Unknown_110 = 110,
    Unknown_111 = 111,
    Unknown_112 = 112,
    Unknown_113 = 113,
    Unknown_114 = 114,
    Unknown_115 = 115,
    Unknown_116 = 116,
    Unknown_117 = 117,
    Unknown_118 = 118,
    Unknown_119 = 119,
    Unknown_120 = 120,
    Unknown_121 = 121,
    Unknown_122 = 122,
    Unknown_123 = 123,
    Unknown_124 = 124,
    Unknown_125 = 125,
    Unknown_126 = 126,
    Unknown_127 = 127,
    Unknown_128 = 128,
    Unknown_129 = 129,
    Unknown_130 = 130,
    Unknown_RF_131 = 131,
    Unknown_RF_132 = 132,
    Unknown_RF_133 = 133,
    Unknown_134 = 134,
    Unknown_135 = 135,
    Unknown_136 = 136,
    Unknown_RF_137 = 137,
    Unknown_138 = 138,
    Unknown_139 = 139,
    Unknown_RF_140 = 140,
    Unknown_141 = 141,
    Unknown_142 = 142,
    Unknown_143 = 143,
    Unknown_144 = 144,
    Unknown_145 = 145,
    Unknown_146 = 146,
    Unknown_147 = 147,
    Unknown_148 = 148,
    Unknown_149 = 149,
    Unknown_150 = 150,
    Unknown_151 = 151,
    Unknown_152 = 152,
    Unknown_153 = 153,
    Unknown_154 = 154,
    Unknown_155 = 155,
    Unknown_156 = 156,
    Unknown_157 = 157,
    Unknown_158 = 158,
    Unknown_159 = 159,
    Unknown_160 = 160,
    Unknown_161 = 161,
    Unknown_162 = 162,
    Unknown_163 = 163,
};

void SsUtAllKeyOff(s32 mode);

void SND_Reset_476BA0();

void SND_Load_VABS(std::shared_ptr<PathSoundInfo>& pSoundBlockInfo, s32 reverb);

void SND_Stop_Channels_Mask(s32 mask);

void SND_Load_Seqs_477AB0(OpenSeqHandle* pSeqTable, std::shared_ptr<PathSoundInfo>& bsqFileName);

s16 SND_SEQ_PlaySeq(SeqId idx, s32 repeatCount, s16 bDontStop);

void SND_SEQ_Stop(SeqId idx);

s16 SND_SEQ_Play(SeqId idx, s32 repeatCount, s16 volLeft, s16 volRight);

s16 SND_SsIsEos_DeInlined(SeqId idx);

s32 SFX_SfxDefinition_Play(const relive::SfxDefinition& sfxDef, s16 volLeft, s16 volRight, s16 pitch_min, s16 pitch_max);

void SND_Init();

void SND_Shutdown();

void SND_SEQ_SetVol(SeqId idx, s16 volLeft, s16 volRight);

void SND_StopAll();

} // namespace AO
