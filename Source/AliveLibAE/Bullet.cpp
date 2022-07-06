#include "stdafx.h"
#include "Abe.hpp"
#include "Bullet.hpp"
#include "Function.hpp"
#include "Map.hpp"
#include "stdlib.hpp"
#include "BaseAliveGameObject.hpp"
#include "Game.hpp"
#include "Slig.hpp"
#include "Spark.hpp"
#include "Sfx.hpp"
#include "Particle.hpp"
#include "ScreenManager.hpp"
#include "Grid.hpp"

Bullet::Bullet(BaseAliveGameObject* pParent, BulletType type, FP xpos, FP ypos, FP xDist, s32 unused, FP scale, s32 numberOfBullets)
    : BaseGameObject(TRUE, 0)
{
    SetType(ReliveTypes::eBullet);
    field_20_type = type;
    field_28_xpos = xpos;
    field_2C_ypos = ypos;
    field_3A_path = gMap.mCurrentPath;
    field_40_pParent = pParent;
    field_38_level = gMap.mCurrentLevel;
    field_3C_scale = scale;
    field_34_unused = unused;
    field_44_number_of_bullets = static_cast<s16>(numberOfBullets);
    field_30_x_distance = xDist;
    field_22_unused = 0;
}

void Bullet::VUpdate()
{
    if (!gMap.Is_Point_In_Current_Camera(field_38_level, field_3A_path, field_28_xpos, field_2C_ypos, 0) && !gMap.Is_Point_In_Current_Camera(field_38_level, field_3A_path, field_28_xpos + FP_FromInteger(10), field_2C_ypos, 0) && !gMap.Is_Point_In_Current_Camera(field_38_level, field_3A_path, field_28_xpos - FP_FromInteger(10), field_2C_ypos, 0))
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
        return;
    }

    const s16 volume = field_3C_scale != FP_FromDouble(0.5) ? 75 : 50;

    switch (field_20_type)
    {
        case BulletType::eSligPossessedOrUnderGlukkonCommand_0:
        case BulletType::eNormalBullet_2:
        {
            s32 randomW = FP_GetExponent(FP_FromInteger(Math_RandomRange(1, 5)) * field_3C_scale);
            const FP randomHeight = FP_FromInteger(Math_RandomRange(1, 5)) * field_3C_scale;

            PSX_RECT shootRect = {};
            if (field_30_x_distance > FP_FromInteger(0))
            {
                shootRect.x = FP_GetExponent(field_28_xpos);
                shootRect.w = FP_GetExponent(pScreenManager->CamXPos() + FP_FromInteger(640));
            }
            else
            {
                shootRect.x = FP_GetExponent(pScreenManager->CamXPos());
                shootRect.w = FP_GetExponent(field_28_xpos);
            }

            shootRect.y = FP_GetExponent(field_2C_ypos - FP_FromInteger(5));
            shootRect.h = FP_GetExponent(field_2C_ypos + FP_FromInteger(5)); // TODO: Check correct

            if (sControlledCharacter->Type() == ReliveTypes::eAbe)
            {
                shootRect.y = FP_GetExponent(field_2C_ypos - FP_FromInteger(10));
            }

            BaseAliveGameObject* pShotObj = ShootObject(&shootRect);

            const s16 vol = field_3C_scale != FP_FromDouble(0.5) ? 90 : 60;

            FP hitX = {};
            FP hitY = {};
            if (sCollisions->Raycast(
                    field_28_xpos,
                    field_2C_ypos - (FP_FromInteger(10) * field_3C_scale),
                    field_30_x_distance + field_28_xpos,
                    field_2C_ypos - (FP_FromInteger(10) * field_3C_scale),
                    &field_24_pLine,
                    &hitX,
                    &hitY,
                    field_3C_scale > FP_FromDouble(0.5) ? kFgFloorCeilingOrWalls : kBgFloorCeilingOrWalls)
                == 1)
            {
                if (pShotObj)
                {
                    if (FP_Abs(pShotObj->mXPos - field_28_xpos) <= FP_Abs(hitX - field_28_xpos))
                    {
                        if (pShotObj->Type() == ReliveTypes::eMineCar || pShotObj->Type() == ReliveTypes::eGreeter)
                        {
                            if (pShotObj->Type() == ReliveTypes::eGreeter)
                            {
                                randomW = FP_GetExponent(FP_FromInteger(randomW) + (FP_FromInteger(14) * field_3C_scale));
                            }

                            if (field_30_x_distance <= FP_FromInteger(0))
                            {
                                relive_new Spark(
                                    pShotObj->mXPos + (field_3C_scale * FP_FromInteger(30)) - FP_FromInteger(randomW),
                                    field_2C_ypos + FP_NoFractional(randomHeight),
                                    field_3C_scale, 6, -76, 76, SparkType::eSmallChantParticle_0);
                                New_Smoke_Particles(
                                    pShotObj->mXPos + (field_3C_scale * FP_FromInteger(30)) - FP_FromInteger(randomW),
                                    field_2C_ypos + FP_NoFractional(randomHeight),
                                    field_3C_scale, 3, 128u, 128u, 128u);
                            }
                            else
                            {
                                relive_new Spark(
                                    pShotObj->mXPos + FP_FromInteger(randomW) - (field_3C_scale * FP_FromInteger(30)),
                                    field_2C_ypos + FP_NoFractional(randomHeight),
                                    field_3C_scale, 6, 50, 205, SparkType::eSmallChantParticle_0);
                                New_Smoke_Particles(
                                    pShotObj->mXPos + FP_FromInteger(randomW) - (field_3C_scale * FP_FromInteger(30)),
                                    field_2C_ypos + FP_NoFractional(randomHeight),
                                    field_3C_scale, 3, 128u, 128u, 128u);
                            }

                            if (Math_RandomRange(0, 100) < 90)
                            {
                                SfxPlayMono(static_cast<SoundEffect>(Math_RandomRange(SoundEffect::Bullet1_0, SoundEffect::Bullet2_1)), volume);
                            }
                        }

                        if (pShotObj->VTakeDamage(this))
                        {
                            if (pShotObj->Type() != ReliveTypes::eGreeter && pShotObj->Type() != ReliveTypes::eMineCar)
                            {
                                PlayBulletSounds(vol);
                            }
                        }
                        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
                        return;
                    }
                }

                if (field_30_x_distance <= FP_FromInteger(0))
                {
                    relive_new Spark(
                        hitX - (field_3C_scale * FP_FromInteger(6)),
                        (FP_FromInteger(10) * field_3C_scale) + hitY,
                        field_3C_scale, 6, -76, 76, SparkType::eSmallChantParticle_0);
                    New_Smoke_Particles(hitX - (field_3C_scale * FP_FromInteger(6)), hitY, field_3C_scale, 3, 128u, 128u, 128u);
                }
                else
                {
                    relive_new Spark(
                        hitX + (field_3C_scale * FP_FromInteger(7)),
                        (FP_FromInteger(10) * field_3C_scale) + hitY,
                        field_3C_scale, 6, 50, 205, SparkType::eSmallChantParticle_0);
                    New_Smoke_Particles(hitX + (field_3C_scale * FP_FromInteger(7)), hitY, field_3C_scale, 3, 128u, 128u, 128u);
                }

                if (Math_RandomRange(0, 100) < 90)
                {
                    SfxPlayMono(static_cast<SoundEffect>(Math_RandomRange(SoundEffect::Bullet1_0, SoundEffect::Bullet2_1)), volume);
                }
                mBaseGameObjectFlags.Set(BaseGameObject::eDead);
                return;
            }

            if (pShotObj)
            {
                if (pShotObj->Type() == ReliveTypes::eMineCar || pShotObj->Type() == ReliveTypes::eGreeter)
                {
                    if (pShotObj->Type() == ReliveTypes::eGreeter)
                    {
                        randomW = FP_GetExponent(FP_FromInteger(randomW) + (FP_FromInteger(14) * field_3C_scale));
                    }

                    if (field_30_x_distance <= FP_FromInteger(0))
                    {

                        relive_new Spark(
                            (field_3C_scale * FP_FromInteger(30)) + pShotObj->mXPos - FP_FromInteger(randomW),
                            field_2C_ypos + FP_NoFractional(randomHeight),
                            field_3C_scale, 6, -76, 76, SparkType::eSmallChantParticle_0);

                        New_Smoke_Particles(
                            (field_3C_scale * FP_FromInteger(30)) + pShotObj->mXPos - FP_FromInteger(randomW),
                            field_2C_ypos + FP_NoFractional(randomHeight),
                            field_3C_scale, 3, 128u, 128u, 128u);
                    }
                    else
                    {

                        relive_new Spark(
                            FP_FromInteger(randomW) + pShotObj->mXPos - (field_3C_scale * FP_FromInteger(30)),
                            field_2C_ypos + FP_NoFractional(randomHeight),
                            field_3C_scale, 6, 50, 205, SparkType::eSmallChantParticle_0);


                        New_Smoke_Particles(
                            FP_FromInteger(randomW) + pShotObj->mXPos - (field_3C_scale * FP_FromInteger(30)),
                            field_2C_ypos + FP_NoFractional(randomHeight),
                            field_3C_scale, 3, 128u, 128u, 128u);
                    }

                    if (Math_RandomRange(0, 100) < 90)
                    {
                        SfxPlayMono(static_cast<SoundEffect>(Math_RandomRange(SoundEffect::Bullet1_0, SoundEffect::Bullet2_1)), volume);
                    }
                }

                if (pShotObj->VTakeDamage(this))
                {
                    if (pShotObj->Type() != ReliveTypes::eGreeter && pShotObj->Type() != ReliveTypes::eMineCar)
                    {
                        PlayBulletSounds(vol);
                    }
                }
                mBaseGameObjectFlags.Set(BaseGameObject::eDead);
                return;
            }

            if (Math_RandomRange(0, 100) < 70)
            {
                SfxPlayMono(static_cast<SoundEffect>(Math_RandomRange(SoundEffect::Bullet1_0, SoundEffect::Bullet2_1)), volume);
            }
            mBaseGameObjectFlags.Set(BaseGameObject::eDead);
            return;
        }

        case BulletType::ePossessedSligZBullet_1:
        {
            const s32 xSnapped = SnapToXGrid(FP_FromInteger(1), FP_GetExponent(sControlledCharacter->mXPos));
            PSX_RECT rect = {};
            rect.x = static_cast<s16>(xSnapped - 25);
            rect.w = static_cast<s16>(xSnapped - 25 + 50);
            rect.y = FP_GetExponent(pScreenManager->CamYPos());
            rect.h = static_cast<s16>(rect.y + 240);
            BaseAliveGameObject* pShootObj = ShootObject(&rect);
            if (pShootObj)
            {
                if (pShootObj->VTakeDamage(this))
                {
                    if (pShootObj->Type() != ReliveTypes::eGreeter && pShootObj->Type() != ReliveTypes::eMineCar)
                    {
                        PlayBulletSounds(90);
                        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
                        return;
                    }
                }
            }

            FP hitX = {};
            FP hitY = {};
            if (sCollisions->Raycast(
                    sControlledCharacter->mXPos,
                    FP_FromInteger(rect.y),
                    sControlledCharacter->mXPos,
                    FP_FromInteger(rect.h),
                    &field_24_pLine, &hitX, &hitY, CollisionMask(eBulletWall_10))
                == 1)
            {
                relive_new Spark(hitX, hitY, FP_FromInteger(1), 9, -31, 159, SparkType::eSmallChantParticle_0);
                New_Smoke_Particles(hitX, hitY, FP_FromInteger(1), 3, 128u, 128u, 128u);
            }
            SfxPlayMono(static_cast<SoundEffect>(Math_RandomRange(SoundEffect::Bullet1_0, SoundEffect::Bullet2_1)), 75);
            mBaseGameObjectFlags.Set(BaseGameObject::eDead);
            return;
        }

        case BulletType::eZBullet_3:
        {
            FP rectXPos = {};
            // TODO: Check field_44_number_of_bullets << 20 is FP_FromInt * 16.
            if (field_28_xpos >= sControlledCharacter->mXPos)
            {
                const FP doubleVelX = (sControlledCharacter->mVelX * FP_FromInteger(2));
                rectXPos = (FP_FromInteger(field_44_number_of_bullets * 16)) + sControlledCharacter->mXPos - doubleVelX;
            }
            else
            {
                const FP doubleVelX = (sControlledCharacter->mVelX * FP_FromInteger(2));
                rectXPos = sControlledCharacter->mXPos - doubleVelX - (FP_FromInteger(field_44_number_of_bullets * 16));
            }

            PSX_RECT rect = sControlledCharacter->VGetBoundingRect();
            rect.x = FP_GetExponent(rectXPos);
            rect.y = rect.h - 10;
            rect.w = rect.x + 2;
            rect.h = rect.h;

            BaseAliveGameObject* pShootObj = ShootObject(&rect);
            if (pShootObj && pShootObj->VTakeDamage(this) && pShootObj->Type() != ReliveTypes::eGreeter && pShootObj->Type() != ReliveTypes::eMineCar)
            {
                PlayBulletSounds(90);
            }
            else
            {
                FP hitX = {};
                FP hitY = {};
                if (sCollisions->Raycast(
                        field_28_xpos,
                        field_2C_ypos,
                        rectXPos,
                        sActiveHero->mYPos + FP_FromInteger(10),
                        &field_24_pLine, &hitX, &hitY, CollisionMask(eBulletWall_10))
                    == 1)
                {
                    relive_new Spark(hitX, hitY, FP_FromInteger(1), 9, -31, 159, SparkType::eSmallChantParticle_0);
                    New_Smoke_Particles(hitX, hitY, FP_FromInteger(1), 3, 128u, 128u, 128u);
                }
                SfxPlayMono(static_cast<SoundEffect>(Math_RandomRange(SoundEffect::Bullet1_0, SoundEffect::Bullet2_1)), 75);
            }
            mBaseGameObjectFlags.Set(BaseGameObject::eDead);
            return;
        }

        default:
            mBaseGameObjectFlags.Set(BaseGameObject::eDead);
            return;
    }
}

bool Bullet::InZBulletCover(FP xpos, FP ypos, const PSX_RECT& objRect)
{
    Path_TLV* pZCover = nullptr;
    while (1)
    {
        // Go to the next entry (or first if first call).
        pZCover = sPathInfo->TlvGetAt(
            pZCover,
            xpos,
            ypos,
            xpos,
            ypos);

        // No more TLVs? Then no Z Cover.
        if (!pZCover)
        {
            break;
        }

        // Is it a Z Cover?
        if (pZCover->mTlvType32 == TlvTypes::ZSligCover_50)
        {
            // Within Z Cover?
            if (objRect.x >= pZCover->mTopLeft.x && objRect.x <= pZCover->mBottomRight.x && objRect.y >= pZCover->mTopLeft.y && objRect.y <= pZCover->mBottomRight.y && objRect.w >= pZCover->mTopLeft.x && objRect.w <= pZCover->mBottomRight.x && objRect.h >= pZCover->mTopLeft.y && objRect.h <= pZCover->mBottomRight.y)
            {
                return true;
            }
        }
    }
    return false;
}

BaseAliveGameObject* Bullet::ShootObject(PSX_RECT* pRect)
{
    if (!gBaseAliveGameObjects_5C1B7C)
    {
        return nullptr;
    }

    BaseAliveGameObject* pObjectToShoot = nullptr;
    for (s32 i = 0; i < gBaseAliveGameObjects_5C1B7C->Size(); i++)
    {
        BaseAliveGameObject* pObj = gBaseAliveGameObjects_5C1B7C->ItemAt(i);
        if (!pObj)
        {
            break;
        }

        if (pObj != field_40_pParent)
        {
            if (pObj->mAnim.mFlags.Get(AnimFlags::eBit3_Render))
            {
                if (gMap.Is_Point_In_Current_Camera(field_38_level, field_3A_path, pObj->mXPos, pObj->mYPos, 1))
                {
                    if (((field_20_type == BulletType::eSligPossessedOrUnderGlukkonCommand_0 || field_20_type == BulletType::ePossessedSligZBullet_1) && ((pObj->Type() == ReliveTypes::eSlig && pObj->mCurrentMotion != eSligMotions::M_Possess_37_4B72C0) || pObj->Type() == ReliveTypes::eFlyingSlig || pObj->Type() == ReliveTypes::eCrawlingSlig || pObj->Type() == ReliveTypes::eGlukkon || pObj->Type() == ReliveTypes::eMudokon || pObj->Type() == ReliveTypes::eAbe || pObj->Type() == ReliveTypes::eSlog || pObj->Type() == ReliveTypes::eGreeter)) ||

                        pObj->Type() == ReliveTypes::eMudokon || pObj->Type() == ReliveTypes::eAbe || pObj->Type() == ReliveTypes::eMineCar || (pObj->Type() == ReliveTypes::eSlig && sControlledCharacter == pObj) || (pObj->Type() == ReliveTypes::eFlyingSlig && sControlledCharacter == pObj) || (pObj->Type() == ReliveTypes::eCrawlingSlig && sControlledCharacter == pObj) || (pObj->Type() == ReliveTypes::eGlukkon && sControlledCharacter == pObj))
                    {
                        const PSX_RECT bRect = pObj->VGetBoundingRect();

                        if (pRect->x <= bRect.w && pRect->w >= bRect.x && pRect->h >= bRect.y && pRect->y <= bRect.h)
                        {
                            if (((field_20_type == BulletType::eZBullet_3 || field_20_type == BulletType::ePossessedSligZBullet_1) && field_40_pParent->mScale < pObj->mScale) || ((field_20_type == BulletType::eNormalBullet_2 || field_20_type == BulletType::eSligPossessedOrUnderGlukkonCommand_0) && field_40_pParent->mScale == pObj->mScale))
                            {
                                if (pObj->Type() != ReliveTypes::eGlukkon || FP_Abs(pObj->mXPos - field_28_xpos) >= ScaleToGridSize(field_3C_scale))
                                {
                                    if (!pObjectToShoot)
                                    {
                                        pObjectToShoot = pObj;
                                    }
                                    else if (FP_Abs(pObj->mXPos - field_28_xpos) < FP_Abs(pObjectToShoot->mXPos - field_28_xpos))
                                    {
                                        pObjectToShoot = pObj;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return pObjectToShoot;
}

void Bullet::PlayBulletSounds(s16 volume)
{
    SFX_Play_Pitch(SoundEffect::AirStream_23, volume, 2000);
    SFX_Play_Pitch(SoundEffect::MeatBounce_36, volume, Math_RandomRange(300, 700));
    SFX_Play_Pitch(SoundEffect::KillEffect_64, volume, Math_RandomRange(900, 1400));
}
