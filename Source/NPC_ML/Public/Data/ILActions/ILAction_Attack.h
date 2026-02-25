#pragma once

#include "CoreMinimal.h"
#include "ILAction_Base.h"
#include "Data/LearningAgentsSchemaKeys.h"

namespace LearningAgentsImitationActions
{
	class NPC_ML_API FAction_Attack : public FAction
	{
		using Super = FAction;
	public:
		FAction_Attack(float Timestamp, uint8 InAttackIndex, const UEnum* InAttackEnum)
			: Super(Timestamp), AttackIndex(InAttackIndex), AttackEnum(InAttackEnum)
		{
			SetType(LAActionKeys::Key_Action_Combat_Attack);
		}
						
		virtual bool IsImmediate() const override { return true; }
		virtual bool CanCombine(FAction* OtherAction) const override { return false; }
		virtual FName GetActionName() const override { return LAActionKeys::Key_Action_Combat_Attack; }
		virtual FLearningAgentsActionObjectElement GetAction(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
						
	private:
		uint8 AttackIndex = 0;
		TStrongObjectPtr<const UEnum> AttackEnum = nullptr;
	};
}
