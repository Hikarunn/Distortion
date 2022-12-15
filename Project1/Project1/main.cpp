#include "DxLib.h"
#include <math.h>

#define CIRCLE_ANGLE_VERTEX_NUM			16			// 円周の頂点数
#define CIRCLE_RADIUS_VERTEX_NUM		8			// 半径の頂点数

#define SCREEN_W	640			// 画面の幅
#define SCREEN_H	480			// 画面の高さ

constexpr double PHI_F = 3.141592;

// 画面を歪ませながら描画する関数
void DrawCircleScreen(
	int CenterX,			// 円の中心座標X
	int CenterY,			// 円の中心座標Y
	float InRadius,			// 内側の円のサイズ
	float OutRadius,		// 外側の円のサイズ
	float Absorption,		// 内側の円に引き込まれるドット数
	int ScreenHandle		// 画面グラフィックハンドル
)
{
	float CenterDistance;
	float GraphCenterDistance;
	float AbsorptionDistance;
	float AbsorptionMoveX, AbsorptionMoveY;
	float Angle;
	int TextureW, TextureH;
	float Sin, Cos;
	COLOR_U8 DiffuseColor;
	int i, j;
	VERTEX2D* Vert;
	WORD* Ind;
	float AngleCosTable[CIRCLE_ANGLE_VERTEX_NUM];
	float AngleSinTable[CIRCLE_ANGLE_VERTEX_NUM];
	float InCircleCosTable[CIRCLE_RADIUS_VERTEX_NUM];

	// スタックに積むには大きいので static 配列にしました
	static VERTEX2D Vertex[CIRCLE_RADIUS_VERTEX_NUM * CIRCLE_ANGLE_VERTEX_NUM];
	static WORD Index[CIRCLE_ANGLE_VERTEX_NUM * 6 * (CIRCLE_RADIUS_VERTEX_NUM - 1)];

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


	// 外側のドーナツ部分を描画

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
			CenterDistance = OutRadius - (OutRadius - InRadius) * j / (CIRCLE_RADIUS_VERTEX_NUM - 1);

			// 中心に引き込まれる距離を算出
			AbsorptionDistance = Absorption * j / (CIRCLE_RADIUS_VERTEX_NUM - 1);

			// 中心に向かって移動する距離を算出
			AbsorptionMoveX = Cos * AbsorptionDistance;
			AbsorptionMoveY = Sin * AbsorptionDistance;

			// スクリーン座標の決定
			Vert->pos.x = Cos * CenterDistance + CenterX;
			Vert->pos.y = Sin * CenterDistance + CenterY;
			Vert->pos.z = 0.0f;

			// テクスチャ座標のセット
			Vert->u = (Vert->pos.x + AbsorptionMoveX) / TextureW;
			Vert->v = (Vert->pos.y + AbsorptionMoveY) / TextureH;

			// その他のパラメータをセット
			Vert->rhw = 1.0f;
			Vert->dif = DiffuseColor;
		}
	}

	// 歪んだドーナツの描画
	DrawPrimitiveIndexed2D(Vertex, sizeof(Vertex) / sizeof(VERTEX2D), Index, sizeof(Index) / sizeof(WORD), DX_PRIMTYPE_TRIANGLELIST, ScreenHandle, FALSE);

	for (i = 0; i < sizeof(Index) / sizeof(WORD); i += 3) {
	 DrawLine(Vertex[Index[i + 0]].pos.x, Vertex[Index[i + 0]].pos.y, Vertex[Index[i + 1]].pos.x, Vertex[Index[i + 1]].pos.y, GetColor(255, 0, 0));
	 DrawLine(Vertex[Index[i + 1]].pos.x, Vertex[Index[i + 1]].pos.y, Vertex[Index[i + 2]].pos.x, Vertex[Index[i + 2]].pos.y, GetColor(255, 0, 0));
	 DrawLine(Vertex[Index[i + 2]].pos.x, Vertex[Index[i + 2]].pos.y, Vertex[Index[i + 0]].pos.x, Vertex[Index[i + 0]].pos.y, GetColor(255, 0, 0));
		
	}

	// 内側の盛り上がっているように見える部分を描画

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
			CenterDistance = InCircleCosTable[j] * InRadius;

			// 画像座標視点での円の中心までの距離を算出
			GraphCenterDistance = ((CIRCLE_RADIUS_VERTEX_NUM - 1) - j) * (Absorption + InRadius) / (CIRCLE_RADIUS_VERTEX_NUM - 1);

			// スクリーン座標の決定
			Vert->pos.x = Cos * CenterDistance + CenterX;
			Vert->pos.y = Sin * CenterDistance + CenterY;
			Vert->pos.z = 0.0f;

			// テクスチャ座標のセット
			Vert->u = (Cos * GraphCenterDistance + CenterX) / TextureW;
			Vert->v = (Sin * GraphCenterDistance + CenterY) / TextureH;

			// その他のパラメータをセット
			Vert->rhw = 1.0f;
			Vert->dif = DiffuseColor;
		}
	}

	// 中心の盛り上がって見える部分を描画
	DrawPrimitiveIndexed2D(Vertex, sizeof(Vertex) / sizeof(VERTEX2D), Index, sizeof(Index) / sizeof(WORD), DX_PRIMTYPE_TRIANGLELIST, ScreenHandle, FALSE);
	//
	for (i = 0; i < sizeof(Index) / sizeof(WORD); i += 3) {
	 DrawLine(Vertex[Index[i + 0]].pos.x, Vertex[Index[i + 0]].pos.y, Vertex[Index[i + 1]].pos.x, Vertex[Index[i + 1]].pos.y, GetColor(255, 0, 0));
	 DrawLine(Vertex[Index[i + 1]].pos.x, Vertex[Index[i + 1]].pos.y, Vertex[Index[i + 2]].pos.x, Vertex[Index[i + 2]].pos.y, GetColor(255, 0, 0));
	 DrawLine(Vertex[Index[i + 2]].pos.x, Vertex[Index[i + 2]].pos.y, Vertex[Index[i + 0]].pos.x, Vertex[Index[i + 0]].pos.y, GetColor(255, 0, 0));

	}
}

// WinMain 関数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int ScreenHandle;
	int GrHandle;
	int GraphX, GraphAddX;
	int CircleX, CircleAddX;
	int CircleAngle;
	int i;

	// ウインドウモードで起動
	ChangeWindowMode(TRUE);

	// ＤＸライブラリの初期化
	if (DxLib_Init() < 0) return -1;

	// 描画可能画像の作成
	ScreenHandle = MakeScreen(SCREEN_W, SCREEN_H);

	// 画像の読み込み
	//GrHandle = LoadGraph("Check01.png");
	GrHandle = LoadGraph("Yakiniku.png");

	// 描画先を裏画面にする
	SetDrawScreen(DX_SCREEN_BACK);

	// 画像と円の推移用パラメータを初期化
	GraphX = 0;
	GraphAddX = 2;
	CircleX = 320;
	CircleAddX = -2;
	CircleAngle = 0;

	// メインループ(何かキーが押されたらループを抜ける)
	while (ProcessMessage() == 0)
	{
		// 画像を移動
		GraphX += GraphAddX;
		if (GraphX < 0 || GraphX > 640 - 119) GraphAddX = -GraphAddX;

		// 歪み円を移動
		CircleX += CircleAddX;
		if (CircleX < 0 || CircleX > 640) CircleAddX = -CircleAddX;

		// 歪み円の円周の値を揺らがせる処理
		CircleAngle += 1;
		if (CircleAngle >= 360) CircleAngle = 0;

		// 描画先を描画可能画像にする
		SetDrawScreen(ScreenHandle);

		// 画面のクリア
		ClearDrawScreen();

		// 画面に格子を描画する
		for (i = 0; i < 640 / 16; i++)
			DrawLine(i * 16 + 1, 0, i * 16 + 1, 480, GetColor(128, 128, 128));

		for (i = 0; i < 480 / 16; i++)
			DrawLine(0, i * 16 + 1, 640, i * 16 + 1, GetColor(128, 128, 128));

		// 画像の描画
		DrawGraph(GraphX, 0, GrHandle, TRUE);

		// 描画先を裏画面にする
		SetDrawScreen(DX_SCREEN_BACK);

		// 画面を歪ませて描画
		DrawCircleScreen(
			CircleX, 240,	// 中心座標
			80.0f + sin(CircleAngle * PHI_F / 180.0f) * 15.0f,	// 内側の円のサイズ
			200.0f + sin(CircleAngle * 2 * PHI_F / 180.0f) * 50.0f,	// 外側の円のサイズ
			48.0f,	// 内側に引き込まれるドット数
			ScreenHandle);

		// 裏画面の内容を表画面に反映
		ScreenFlip();
	}

	// ＤＸライブラリの後始末
	DxLib_End();

	// ソフトの終了
	return 0;
}