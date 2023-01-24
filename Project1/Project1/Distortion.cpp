#include <DxLib.h>
#include <math.h>
#include "Distortion.h"

void Distortion::DrawCircleScreen(int CenterX, int CenterY, float InRadius, float OutRadius, float Absorption, int ScreenHandle)
{
	centerX_ = CenterX;					// 円の中心座標X
	centerY_ = CenterY;					// 円の中心座標Y	
	screenHandle_ = ScreenHandle;		// 画面グラフィックハンドル	
	inRadius_ = InRadius;				// 内側の円のサイズ
	outRadius_ = OutRadius;				// 外側の円のサイズ
	absorption_ = Absorption;			// 内側の円に引き込まれるドット数

	
	// 描画可能画像が使用しているテクスチャのサイズを取得しておく
	GetGraphTextureSize(ScreenHandle, &TextureW, &TextureH);

	// 最初に普通に描画
	DrawGraph(0, 0, ScreenHandle, FALSE);

	// 描画カラーを作成しておく
	DiffuseColor = GetColorU8(255, 255, 255, 255);

	// 外周部分用の Sin, Cos テーブルを作成する
	Angle = 0.0f;
	for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; i++, Angle += PHI_F * 2.0f / CIRCLE_ANGLE_VERTEX_NUM)
	{
		AngleSinTable[i] = (float)sin((double)Angle);
		AngleCosTable[i] = (float)cos((double)Angle);
	}

	// 内側の盛り上がっているように見せる箇所で使用する Cos テーブルを作成する
	Angle = 0.0f;
	for (i = 0; i < CIRCLE_RADIUS_VERTEX_NUM; i++, Angle += (PHI_F / 2.0f) / (CIRCLE_RADIUS_VERTEX_NUM - 1))
	{
		InCircleCosTable[i] = (float)cos((double)Angle);
	}

	// ポリゴン頂点インデックスの準備
	Ind = Index;
	for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; i++)
	{
		for (j = 0; j < CIRCLE_RADIUS_VERTEX_NUM - 1; j++, Ind += 6)
		{
			Ind[0] = i * CIRCLE_RADIUS_VERTEX_NUM + j;
			Ind[1] = Ind[0] + 1;
			if (i == CIRCLE_ANGLE_VERTEX_NUM - 1)
			{
				Ind[2] = j;
				Ind[3] = j + 1;
			}
			else
			{
				Ind[2] = Ind[0] + CIRCLE_RADIUS_VERTEX_NUM;
				Ind[3] = Ind[0] + 1 + CIRCLE_RADIUS_VERTEX_NUM;
			}
			Ind[4] = Ind[2];
			Ind[5] = Ind[1];
		}
	}

	// バイリニア補間描画にする
	SetDrawMode(DX_DRAWMODE_BILINEAR);


	OuterCircle(centerX_,centerY_,inRadius_,outRadius_,absorption_);
	
	// 歪んだドーナツの描画
	DrawPrimitiveIndexed2D(Vertex, sizeof(Vertex) / sizeof(VERTEX2D), Index, sizeof(Index) / sizeof(WORD), DX_PRIMTYPE_TRIANGLELIST, ScreenHandle, FALSE);

	for (i = 0; i < sizeof(Index) / sizeof(WORD); i += 3) {
		DrawLine(Vertex[Index[i + 0]].pos.x, Vertex[Index[i + 0]].pos.y, Vertex[Index[i + 1]].pos.x, Vertex[Index[i + 1]].pos.y, GetColor(255, 0, 0));
		DrawLine(Vertex[Index[i + 1]].pos.x, Vertex[Index[i + 1]].pos.y, Vertex[Index[i + 2]].pos.x, Vertex[Index[i + 2]].pos.y, GetColor(255, 0, 0));
		DrawLine(Vertex[Index[i + 2]].pos.x, Vertex[Index[i + 2]].pos.y, Vertex[Index[i + 0]].pos.x, Vertex[Index[i + 0]].pos.y, GetColor(255, 0, 0));

	}

	
	InnerCircle(centerX_, centerY_, inRadius_, absorption_);
	// 中心の盛り上がって見える部分を描画
	DrawPrimitiveIndexed2D(Vertex, sizeof(Vertex) / sizeof(VERTEX2D), Index, sizeof(Index) / sizeof(WORD), DX_PRIMTYPE_TRIANGLELIST, ScreenHandle, FALSE);
	//
	for (i = 0; i < sizeof(Index) / sizeof(WORD); i += 3) {
		DrawLine(Vertex[Index[i + 0]].pos.x, Vertex[Index[i + 0]].pos.y, Vertex[Index[i + 1]].pos.x, Vertex[Index[i + 1]].pos.y, GetColor(255, 0, 0));
		DrawLine(Vertex[Index[i + 1]].pos.x, Vertex[Index[i + 1]].pos.y, Vertex[Index[i + 2]].pos.x, Vertex[Index[i + 2]].pos.y, GetColor(255, 0, 0));
		DrawLine(Vertex[Index[i + 2]].pos.x, Vertex[Index[i + 2]].pos.y, Vertex[Index[i + 0]].pos.x, Vertex[Index[i + 0]].pos.y, GetColor(255, 0, 0));

	}
}

void Distortion::InnerCircle(int centerX, int centerY, float inRadius, float absorption)
{
	// Cosテーブルにしたがってテクスチャ座標をずらす
	Vert = Vertex;
	for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; i++)
	{
		// 使用する Sin, Cos の値をセット
		Sin = AngleSinTable[i];
		Cos = AngleCosTable[i];

		for (j = 0; j < CIRCLE_RADIUS_VERTEX_NUM; j++, Vert++)
		{
			// 円の中心までの距離を算出
			CenterDistance = InCircleCosTable[j] * inRadius;

			// 画像座標視点での円の中心までの距離を算出
			GraphCenterDistance = ((CIRCLE_RADIUS_VERTEX_NUM - 1) - j) * (absorption + inRadius) / (CIRCLE_RADIUS_VERTEX_NUM - 1);

			// スクリーン座標の決定
			Vert->pos.x = Cos * CenterDistance + centerX;
			Vert->pos.y = Sin * CenterDistance + centerY;
			Vert->pos.z = 0.0f;

			// テクスチャ座標のセット
			Vert->u = (Cos * GraphCenterDistance + centerX) / TextureW;
			Vert->v = (Sin * GraphCenterDistance + centerY) / TextureH;

			// その他のパラメータをセット
			Vert->rhw = 1.0f;
			Vert->dif = DiffuseColor;
		}
	}

}

void Distortion::OuterCircle(int centerX, int centerY, float inRadius, float outRadius, float absorption)
{
	// 中心に向かうにしたがって中心方向にテクスチャ座標をずらす
	Vert = Vertex;
	for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; i++)
	{
		// 使用する Sin, Cos の値をセット
		Sin = AngleSinTable[i];
		Cos = AngleCosTable[i];

		for (j = 0; j < CIRCLE_RADIUS_VERTEX_NUM; j++, Vert++)
		{
			// 円の中心までの距離を算出
			CenterDistance = outRadius - (outRadius - inRadius) * j / (CIRCLE_RADIUS_VERTEX_NUM - 1);

			// 中心に引き込まれる距離を算出
			AbsorptionDistance = absorption * j / (CIRCLE_RADIUS_VERTEX_NUM - 1);

			// 中心に向かって移動する距離を算出
			AbsorptionMoveX = Cos * AbsorptionDistance;
			AbsorptionMoveY = Sin * AbsorptionDistance;

			// スクリーン座標の決定
			Vert->pos.x = Cos * CenterDistance + centerX;
			Vert->pos.y = Sin * CenterDistance + centerY;
			Vert->pos.z = 0.0f;

			// テクスチャ座標のセット
			Vert->u = (Vert->pos.x + AbsorptionMoveX) / TextureW;
			Vert->v = (Vert->pos.y + AbsorptionMoveY) / TextureH;

			// その他のパラメータをセット
			Vert->rhw = 1.0f;
			Vert->dif = DiffuseColor;
		}
	}

}

