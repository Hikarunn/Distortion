#pragma once

constexpr int CIRCLE_ANGLE_VERTEX_NUM = 16;			// �~���̒��_��
constexpr int CIRCLE_RADIUS_VERTEX_NUM = 8;			// ���a�̒��_��
constexpr double PHI_F = 3.141592;					// �~����

// �X�^�b�N�ɐςނɂ͑傫���̂� static �z��ɂ��܂���
static VERTEX2D Vertex[CIRCLE_RADIUS_VERTEX_NUM * CIRCLE_ANGLE_VERTEX_NUM];
static WORD Index[CIRCLE_ANGLE_VERTEX_NUM * 6 * (CIRCLE_RADIUS_VERTEX_NUM - 1)];


class Distortion
{
public:
	void DrawCircleScreen(
		int CenterX,			// �~�̒��S���WX
		int CenterY,			// �~�̒��S���WY
		float InRadius,			// �����̉~�̃T�C�Y
		float OutRadius,		// �O���̉~�̃T�C�Y
		float Absorption,		// �����̉~�Ɉ������܂��h�b�g��
		int ScreenHandle		// ��ʃO���t�B�b�N�n���h��
	);
private:

	// �����̐���オ���Ă���悤�Ɍ����镔����`��
	void InnerCircle(int centerX, int centerY, float inRadius,float absorptions );

	// �O���̃h�[�i�c������`��
	void OuterCircle(int centerX, int centerY, float inRadius, float outRadius, float absorptions);

	int centerX_;			// �~�̒��S���WX
	int centerY_;			// �~�̒��S���WY	
	int screenHandle_;		// ��ʃO���t�B�b�N�n���h��	
	float inRadius_;		// �����̉~�̃T�C�Y
	float outRadius_;		// �O���̉~�̃T�C�Y
	float absorption_;		// �����̉~�Ɉ������܂��h�b�g��

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

