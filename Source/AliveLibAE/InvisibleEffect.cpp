#include "stdafx.h"
#include "InvisibleEffect.hpp"
#include "Function.hpp"
#include "PossessionFlicker.hpp"
#include "../relive_lib/Events.hpp"
#include "../relive_lib/ObjectIds.hpp"
#include "BaseAliveGameObject.hpp"
#include "stdlib.hpp"

InvisibleEffect::InvisibleEffect(BaseAliveGameObject* pTarget)
    : BaseGameObject(true, 0)
{
     SetType(ReliveTypes::eInvisibleEffect);

    field_44_objId = pTarget->mBaseGameObjectId;

    mPal1.mPal = std::make_shared<AnimationPal>(*pTarget->GetAnimation().mAnimRes.mTgaPtr->mPal);
    mPal2.mPal = std::make_shared<AnimationPal>(*pTarget->GetAnimation().mAnimRes.mTgaPtr->mPal);

    field_4A_flags.Clear();

    if (pTarget->GetAnimation().mFlags.Get(AnimFlags::eSemiTrans))
    {
        field_4A_flags.Set(Flags_4A::eSemiTrans_Bit1);
    }
    if (pTarget->GetAnimation().mFlags.Get(AnimFlags::eBlending))
    {
        field_4A_flags.Set(Flags_4A::eBlending_Bit2);
    }

    field_4A_flags.Clear(Flags_4A::eIsInvisible_Bit3);
    field_48_old_render_mode = pTarget->GetAnimation().GetRenderMode();
    field_20_state_or_op = InvisibleState::eSetRenderMode1_0;
}

InvisibleEffect::~InvisibleEffect()
{

}

void InvisibleEffect::InstantInvisibility()
{
    field_4A_flags.Set(Flags_4A::eIsInvisible_Bit3);
    SetUpdateDelay(1);
    field_20_state_or_op = InvisibleState::eSetInvisibile_1;
}

void InvisibleEffect::BecomeVisible()
{
    field_20_state_or_op = InvisibleState::eBecomeVisible_4;
}

void InvisibleEffect::ClearInvisibility()
{
    SetUpdateDelay(1);
    field_20_state_or_op = InvisibleState::eClearInvisibility_5;
}

void InvisibleEffect::BecomeInvisible()
{
    SetUpdateDelay(1);
    field_20_state_or_op = InvisibleState::eSetInvisibile_1;
}

void InvisibleEffect::VUpdate()
{
    auto pTarget = static_cast<BaseAliveGameObject*>(sObjectIds.Find_Impl(field_44_objId));
    if (EventGet(kEventDeathReset))
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }

    if (!pTarget || pTarget->mBaseGameObjectFlags.Get(BaseGameObject::eDead))
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }
    else
    {
        switch (field_20_state_or_op)
        {
            case InvisibleState::eSetRenderMode1_0:
            {
                pTarget->GetAnimation().SetRenderMode(TPageAbr::eBlend_1);
                return;
            }
            case InvisibleState::eSetInvisibile_1:
            {
                for (s32 idx2 = 8; idx2 < 256; idx2++)
                {
                    // Set transparent bit
                    mPal2.mPal->mPal[idx2].a = 255;
                }

                pTarget->mBaseAliveGameObjectFlags.Set(AliveObjectFlags::eInvisible);

                pTarget->GetAnimation().mFlags.Clear(AnimFlags::eBlending);
                pTarget->GetAnimation().mFlags.Set(AnimFlags::eSemiTrans);
                pTarget->GetAnimation().SetRenderMode(TPageAbr::eBlend_1);

                SetUpdateDelay(1);
                field_20_state_or_op = InvisibleState::eBecomeInvisible_2;
                if (field_4A_flags.Get(Flags_4A::eIsInvisible_Bit3))
                {
                    field_20_state_or_op = InvisibleState::eUnknown_3;
                }
                return;
            }
            case InvisibleState::eBecomeInvisible_2:
            {
                /* - should never have been possible
                if (pTarget->mAnim.mPalDepth <= 8)
                {
                    field_20_state_or_op = InvisibleState::eSetRenderMode1_0;
                    return;
                }*/

                bool v3 = false;
                for (s32 idx = 8; idx < 256; idx++)
                {
                    // Red
                    if (mPal2.mPal->mPal[idx].r)
                    {
                        v3 = true;
                        mPal2.mPal->mPal[idx].r -= 1;
                    }

                    // Green
                    if (mPal2.mPal->mPal[idx].g)
                    {
                        v3 = true;
                        mPal2.mPal->mPal[idx].g -= 1;
                    }

                    // Blue
                    if (mPal2.mPal->mPal[idx].b)
                    {
                        v3 = true;
                        mPal2.mPal->mPal[idx].b -= 1;
                    }

                    // Semi trans
                    if (mPal2.mPal->mPal[idx].a == 255 && mPal2.mPal->mPal[idx].r == 0 && mPal2.mPal->mPal[idx].g == 0 && mPal2.mPal->mPal[idx].b == 0)
                    {
                        mPal2.mPal->mPal[idx].a = 0;
                    }
                }

                if (!v3)
                {
                    field_20_state_or_op = InvisibleState::eSetRenderMode1_0;
                }
                else
                {
                    // TODO: Allow setting anim
                    pTarget->GetAnimation().LoadPal(mPal2);
                    SetUpdateDelay(1);
                }


                break;
            }
            case InvisibleState::eUnknown_3:
            {
                LOG_ERROR("Unreachable case InvisibleState::eUnknown_3 hit");
                for (s32 i = 8; i < 256; i++)
                {
                    // Clear transparent bit
                    mPal2.mPal->mPal[i].a = 0;
                }
                // TODO
                pTarget->GetAnimation().LoadPal(mPal2);
                //Pal_Set(pTarget->mAnim.mPalVramXY, pTarget->mAnim.mPalDepth, (u8*) field_30_pPal2, &field_34_pal_rect2);

                field_4A_flags.Clear(Flags_4A::eIsInvisible_Bit3);
                SetUpdateDelay(1);
                field_20_state_or_op = InvisibleState::eSetRenderMode1_0;
                break;
            }
            case InvisibleState::eBecomeVisible_4:
            {
                /* TODO - shouldn't be possible
                if (pTarget->mAnim.mPalDepth <= 1)
                {
                    field_20_state_or_op = InvisibleState::eClearInvisibility_5;
                    return;
                }*/

                bool v3 = false;
                for (s32 idx4 = 1; idx4 < 256; idx4++)
                {
                    u32* pal1u32 = reinterpret_cast<u32*>(&mPal1.mPal->mPal[idx4]);
                    u32* pal2u32 = reinterpret_cast<u32*>(&mPal2.mPal->mPal[idx4]);
                    u32 palXor = (*pal1u32) ^ (*pal1u32);

                    if (palXor & 0xFF)
                    {
                        v3 = true;
                        mPal2.mPal->mPal[idx4].r += 1;
                    }

                    if (palXor & 0xFF00)
                    {
                        v3 = true;
                        mPal2.mPal->mPal[idx4].g += 1;
                    }

                    if (palXor & 0xFF0000)
                    {
                        v3 = true;
                        mPal2.mPal->mPal[idx4].b += 1;
                    }
                }

                if (v3)
                {
                    // TODO
                    pTarget->GetAnimation().LoadPal(mPal2);

                    //Pal_Set(pTarget->mAnim.mPalVramXY, pTarget->mAnim.mPalDepth, (u8*) field_30_pPal2, &field_34_pal_rect2);

                    pTarget->GetAnimation().SetRenderMode(TPageAbr::eBlend_1);
                    SetUpdateDelay(5);
                }
                else
                {
                    field_20_state_or_op = InvisibleState::eClearInvisibility_5;
                }
                break;
            }
            case InvisibleState::eClearInvisibility_5:
            {
                // TODO
                 pTarget->GetAnimation().LoadPal(mPal1);

                //Pal_Set(pTarget->mAnim.mPalVramXY, pTarget->mAnim.mPalDepth, (u8*) field_24_pPal1, &field_28_pal_rect1);

                pTarget->GetAnimation().mFlags.Set(AnimFlags::eSemiTrans, field_4A_flags.Get(Flags_4A::eSemiTrans_Bit1));
                pTarget->GetAnimation().mFlags.Set(AnimFlags::eBlending, field_4A_flags.Get(Flags_4A::eBlending_Bit2));
                pTarget->GetAnimation().SetRenderMode(field_48_old_render_mode);

                pTarget->mBaseAliveGameObjectFlags.Clear(AliveObjectFlags::eInvisible);

                SetUpdateDelay(1);
                relive_new PossessionFlicker(pTarget, 16, 255, 128, 128);
                field_20_state_or_op = InvisibleState::eSetDead_6;
                break;
            }
            case InvisibleState::eSetDead_6:
            {
                mBaseGameObjectFlags.Set(BaseGameObject::eDead);
                return;
            }
            default:
            {
                return;
            }
        }
    }
}

void InvisibleEffect::VScreenChanged()
{
    // Null @ 45F9C0
}
