#include <DxLib.h>
#include <math.h>
#include "Distortion.h"

void Distortion::DrawCircleScreen(int CenterX, int CenterY, float InRadius, float OutRadius, float Absorption, int ScreenHandle)
{
	centerX_ = CenterX;					// �~�̒��S���WX
	centerY_ = CenterY;					// �~�̒��S���WY	
	screenHandle_ = ScreenHandle;		// ��ʃO���t�B�b�N�n���h��	
	inRadius_ = InRadius;				// �����̉~�̃T�C�Y
	outRadius_ = OutRadius;				// �O���̉~�̃T�C�Y
	absorption_ = Absorption;			// �����̉~�Ɉ������܂��h�b�g��

	
	// �`��\�摜���g�p���Ă���e�N�X�`���̃T�C�Y���擾���Ă���
	GetGraphTextureSize(ScreenHandle, &TextureW, &TextureH);

	// �ŏ��ɕ��ʂɕ`��
	DrawGraph(0, 0, ScreenHandle, FALSE);

	// �`��J���[���쐬���Ă���
	DiffuseColor = GetColorU8(255, 255, 255, 255);

	// �O�������p�� Sin, Cos �e�[�u�����쐬����
	Angle = 0.0f;
	for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; i++, Angle += PHI_F * 2.0f / CIRCLE_ANGLE_VERTEX_NUM)
	{
		AngleSinTable[i] = (float)sin((double)Angle);
		AngleCosTable[i] = (float)cos((double)Angle);
	}

	// �����̐���オ���Ă���悤�Ɍ�����ӏ��Ŏg�p���� Cos �e�[�u�����쐬����
	Angle = 0.0f;
	for (i = 0; i < CIRCLE_RADIUS_VERTEX_NUM; i++, Angle += (PHI_F / 2.0f) / (CIRCLE_RADIUS_VERTEX_NUM - 1))
	{
		InCircleCosTable[i] = (float)cos((double)Angle);
	}

	// �|���S�����_�C���f�b�N�X�̏���
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

	// �o�C���j�A��ԕ`��ɂ���
	SetDrawMode(DX_DRAWMODE_BILINEAR);


	OuterCircle(centerX_,centerY_,inRadius_,outRadius_,absorption_);
	
	// �c�񂾃h�[�i�c�̕`��
	DrawPrimitiveIndexed2D(Vertex, sizeof(Vertex) / sizeof(VERTEX2D), Index, sizeof(Index) / sizeof(WORD), DX_PRIMTYPE_TRIANGLELIST, ScreenHandle, FALSE);

	for (i = 0; i < sizeof(Index) / sizeof(WORD); i += 3) {
		DrawLine(Vertex[Index[i + 0]].pos.x, Vertex[Index[i + 0]].pos.y, Vertex[Index[i + 1]].pos.x, Vertex[Index[i + 1]].pos.y, GetColor(255, 0, 0));
		DrawLine(Vertex[Index[i + 1]].pos.x, Vertex[Index[i + 1]].pos.y, Vertex[Index[i + 2]].pos.x, Vertex[Index[i + 2]].pos.y, GetColor(255, 0, 0));
		DrawLine(Vertex[Index[i + 2]].pos.x, Vertex[Index[i + 2]].pos.y, Vertex[Index[i + 0]].pos.x, Vertex[Index[i + 0]].pos.y, GetColor(255, 0, 0));

	}

	
	InnerCircle(centerX_, centerY_, inRadius_, absorption_);
	// ���S�̐���オ���Č����镔����`��
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
	// Cos�e�[�u���ɂ��������ăe�N�X�`�����W�����炷
	Vert = Vertex;
	for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; i++)
	{
		// �g�p���� Sin, Cos �̒l���Z�b�g
		Sin = AngleSinTable[i];
		Cos = AngleCosTable[i];

		for (j = 0; j < CIRCLE_RADIUS_VERTEX_NUM; j++, Vert++)
		{
			// �~�̒��S�܂ł̋������Z�o
			CenterDistance = InCircleCosTable[j] * inRadius;

			// �摜���W���_�ł̉~�̒��S�܂ł̋������Z�o
			GraphCenterDistance = ((CIRCLE_RADIUS_VERTEX_NUM - 1) - j) * (absorption + inRadius) / (CIRCLE_RADIUS_VERTEX_NUM - 1);

			// �X�N���[�����W�̌���
			Vert->pos.x = Cos * CenterDistance + centerX;
			Vert->pos.y = Sin * CenterDistance + centerY;
			Vert->pos.z = 0.0f;

			// �e�N�X�`�����W�̃Z�b�g
			Vert->u = (Cos * GraphCenterDistance + centerX) / TextureW;
			Vert->v = (Sin * GraphCenterDistance + centerY) / TextureH;

			// ���̑��̃p�����[�^���Z�b�g
			Vert->rhw = 1.0f;
			Vert->dif = DiffuseColor;
		}
	}

}

void Distortion::OuterCircle(int centerX, int centerY, float inRadius, float outRadius, float absorption)
{
	// ���S�Ɍ������ɂ��������Ē��S�����Ƀe�N�X�`�����W�����炷
	Vert = Vertex;
	for (i = 0; i < CIRCLE_ANGLE_VERTEX_NUM; i++)
	{
		// �g�p���� Sin, Cos �̒l���Z�b�g
		Sin = AngleSinTable[i];
		Cos = AngleCosTable[i];

		for (j = 0; j < CIRCLE_RADIUS_VERTEX_NUM; j++, Vert++)
		{
			// �~�̒��S�܂ł̋������Z�o
			CenterDistance = outRadius - (outRadius - inRadius) * j / (CIRCLE_RADIUS_VERTEX_NUM - 1);

			// ���S�Ɉ������܂�鋗�����Z�o
			AbsorptionDistance = absorption * j / (CIRCLE_RADIUS_VERTEX_NUM - 1);

			// ���S�Ɍ������Ĉړ����鋗�����Z�o
			AbsorptionMoveX = Cos * AbsorptionDistance;
			AbsorptionMoveY = Sin * AbsorptionDistance;

			// �X�N���[�����W�̌���
			Vert->pos.x = Cos * CenterDistance + centerX;
			Vert->pos.y = Sin * CenterDistance + centerY;
			Vert->pos.z = 0.0f;

			// �e�N�X�`�����W�̃Z�b�g
			Vert->u = (Vert->pos.x + AbsorptionMoveX) / TextureW;
			Vert->v = (Vert->pos.y + AbsorptionMoveY) / TextureH;

			// ���̑��̃p�����[�^���Z�b�g
			Vert->rhw = 1.0f;
			Vert->dif = DiffuseColor;
		}
	}

}

