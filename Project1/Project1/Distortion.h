#pragma once

constexpr int CIRCLE_ANGLE_VERTEX_NUM = 16;			// 円周の頂点数
constexpr int CIRCLE_RADIUS_VERTEX_NUM = 8;			// 半径の頂点数
constexpr double PHI_F = 3.141592;					// 円周率

// スタックに積むには大きいので static 配列にしました
static VERTEX2D Vertex[CIRCLE_RADIUS_VERTEX_NUM * CIRCLE_ANGLE_VERTEX_NUM];
static WORD Index[CIRCLE_ANGLE_VERTEX_NUM * 6 * (CIRCLE_RADIUS_VERTEX_NUM - 1)];


class Distortion
{
public:
	void DrawCircleScreen(
		int CenterX,			// 円の中心座標X
		int CenterY,			// 円の中心座標Y
		float InRadius,			// 内側の円のサイズ
		float OutRadius,		// 外側の円のサイズ
		float Absorption,		// 内側の円に引き込まれるドット数
		int ScreenHandle		// 画面グラフィックハンドル
	);
private:

	// 内側の盛り上がっているように見える部分を描画
	void InnerCircle(int centerX, int centerY, float inRadius,float absorptions );

	// 外側のドーナツ部分を描画
	void OuterCircle(int centerX, int centerY, float inRadius, float outRadius, float absorptions);

	int centerX_;			// 円の中心座標X
	int centerY_;			// 円の中心座標Y	
	int screenHandle_;		// 画面グラフィックハンドル	
	float inRadius_;		// 内側の円のサイズ
	float outRadius_;		// 外側の円のサイズ
	float absorption_;		// 内側の円に引き込まれるドット数

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
};

