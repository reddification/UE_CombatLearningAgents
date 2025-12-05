#pragma once

#include "GameplayTagContainer.h"
#include "LearningAgentsActions.h"
#include "LearningAgentsDataTypes.h"
#include "LearningAgentsSchemaKeys.h"

namespace LearningAgentsImitationActions
{
	
	class FAction
	{
		public:
			FAction(float Timestamp) : Timestamp(Timestamp) {};
			virtual ~FAction() = default;
		
			virtual bool CanCombine(FAction* OtherAction) const;
			// if true, action doesn't wait for other actions to combine
			virtual bool IsImmediate() const { return false; }
			virtual void Combine(const TSharedPtr<FAction>& OtherAction) { unimplemented(); };
			virtual FLearningAgentsActionObjectElement GetAction(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const = 0;
			virtual FName GetActionName() const = 0;	
		
		protected:
			float Timestamp = 0.f;
			// TArray<FName> Hierarchy; // not sure if it worth the effort
	};

	class FAction_Locomotion_NonBlocking : public FAction
	{
		public:
			FAction_Locomotion_NonBlocking(float Timestamp) : FAction(Timestamp) {};
			virtual ~FAction_Locomotion_NonBlocking() override;
			virtual FLearningAgentsActionObjectElement GetAction(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
			virtual FLearningAgentsActionObjectElement GetActionInternal(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const = 0;
			virtual bool CanCombine(FAction* OtherAction) const override;
			virtual void Combine(const TSharedPtr<FAction>& OtherAction) override;
			virtual FName GetActionName() const override { return LAActionKeys::Key_Action_Locomotion_NonBlocking; }
		
		private:
			TMap<FName, TSharedPtr<FAction>> PendingSimultaneousActions;
	};

	class FAction_Locomotion_Move : public FAction_Locomotion_NonBlocking
	{
		public:
			FAction_Locomotion_Move(float Timestamp, const FVector& InDirection) 
				: FAction_Locomotion_NonBlocking(Timestamp), Direction(InDirection)
			{
			}
			
			virtual FLearningAgentsActionObjectElement GetActionInternal(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
			virtual FName GetActionName() const override { return LAActionKeys::Key_Action_Locomotion_Move; }; 
			virtual bool CanCombine(FAction* OtherAction) const override;	
			virtual void Combine(const TSharedPtr<FAction>& OtherAction) override;
		
		private:
			FVector Direction = FVector::ZeroVector;
	};
	
	class FAction_Locomotion_SetSpeed : public FAction_Locomotion_NonBlocking
	{
		public:
			FAction_Locomotion_SetSpeed(float Timestamp, float NewSpeed) 
				: FAction_Locomotion_NonBlocking(Timestamp), Speed(NewSpeed)
			{
			}
				
			virtual FLearningAgentsActionObjectElement GetActionInternal(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
			virtual FName GetActionName() const override { return LAActionKeys::Key_Action_Locomotion_SetSpeed; }; 
			virtual bool CanCombine(FAction* OtherAction) const override;	
			virtual void Combine(const TSharedPtr<FAction>& OtherAction) override;	
		
		private:
			float Speed = 0.f;
	};

	class FAction_Locomotion_Rotate : public FAction_Locomotion_NonBlocking
	{
		public:
			FAction_Locomotion_Rotate(float Timestamp, const FRotator& Rotation) 
				: FAction_Locomotion_NonBlocking(Timestamp), Rotator(Rotation)
			{
			}
				
			virtual bool CanCombine(FAction* OtherAction) const override { return false; };
			virtual FName GetActionName() const override { return LAActionKeys::Key_Action_Locomotion_Rotate; };
			virtual FLearningAgentsActionObjectElement GetActionInternal(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
				
		private:
			FRotator Rotator = FRotator::ZeroRotator;
	};
	
	class FAction_Locomotion_Phrase : public FAction_Locomotion_NonBlocking
	{
		public:
			FAction_Locomotion_Phrase(float Timestamp, const FGameplayTag& InPhraseTag)
				: FAction_Locomotion_NonBlocking(Timestamp), PhraseTag(InPhraseTag) { }
					
			// virtual bool CanCombine(FAction* OtherAction) const override { return false; }
			virtual FName GetActionName() const override { return LAActionKeys::Key_Action_SayPhrase; };
			virtual FLearningAgentsActionObjectElement GetActionInternal(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
					
		private:
			FGameplayTag PhraseTag;
	};

	class FAction_Locomotion_Animation : public FAction_Locomotion_NonBlocking
	{
		public:
			FAction_Locomotion_Animation(float Timestamp) : FAction_Locomotion_NonBlocking(Timestamp) { }
		
			virtual bool CanCombine(FAction* OtherAction) const override;
			virtual FName GetActionName() const override final { return LAActionKeys::Key_Action_Locomotion_NonBlocking_Animation; };
			virtual FName GetAnimationActionName() const = 0;
			virtual FLearningAgentsActionObjectElement GetActionInternal(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
			virtual FLearningAgentsActionObjectElement GetAnimationAction(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const = 0;
	};
	
	class FAction_Locomotion_Gesture : public FAction_Locomotion_Animation
	{
		public:
			FAction_Locomotion_Gesture(float Timestamp, const FGameplayTag& InGestureTag) 
				: FAction_Locomotion_Animation(Timestamp), GestureTag(InGestureTag) { }
						
			virtual FName GetAnimationActionName() const override { return LAActionKeys::Key_Action_Gesture; }
			virtual FLearningAgentsActionObjectElement GetAnimationAction(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
						
		private:
			FGameplayTag GestureTag;
	};
	
	class FAction_Locomotion_UseConsumableItem : public FAction_Locomotion_Animation
	{
		public:
			FAction_Locomotion_UseConsumableItem(float Timestamp, const FGameplayTag& ItemTag)
				: FAction_Locomotion_Animation(Timestamp), ItemTag(ItemTag) { }
					
			virtual FName GetAnimationActionName() const override { return LAActionKeys::Key_Action_UseConsumableItem; }
			virtual FLearningAgentsActionObjectElement GetAnimationAction(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
					
		private:
			FGameplayTag ItemTag;
	};
	
	class FAction_ChangeWeaponState : public FAction_Locomotion_Animation
	{
		public:
			FAction_ChangeWeaponState(float Timestamp, ELAWeaponStateChange NewWeaponState)
				: FAction_Locomotion_Animation(Timestamp), WeaponState(NewWeaponState) { }
						
			virtual FName GetAnimationActionName() const override { return LAActionKeys::Key_Action_ChangeWeaponState; }
			virtual FLearningAgentsActionObjectElement GetAnimationAction(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
						
		private:
			ELAWeaponStateChange WeaponState;
	};
	
	class FAction_Locomotion_BlockingLocomotion : public FAction
	{
		public:
			FAction_Locomotion_BlockingLocomotion(float Timestamp, ELALocomotionAction LocomotionAction) 
				: FAction(Timestamp), LocomotionAction(LocomotionAction) { }
					
			virtual bool IsImmediate() const override { return true; }
			virtual FName GetActionName() const override { return LAActionKeys::Key_Action_Locomotion_Blocking; }
			virtual bool CanCombine(FAction* OtherAction) const override { return false; }
			virtual FLearningAgentsActionObjectElement GetAction(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
					
		private:
			ELALocomotionAction LocomotionAction;
	};
	
	class FAction_Attack : public FAction
	{
		public:
			FAction_Attack(float Timestamp, int InAttackIndex)
				: FAction(Timestamp), AttackIndex(InAttackIndex) { }
						
			virtual bool IsImmediate() const override { return true; }
			virtual bool CanCombine(FAction* OtherAction) const override { return false; }
			virtual FName GetActionName() const override { return LAActionKeys::Key_Action_Combat_Attack; }
			virtual FLearningAgentsActionObjectElement GetAction(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
						
		private:
			int AttackIndex = 0;
	};
	
	class FAction_Parry : public FAction
	{
		public:
			FAction_Parry(float Timestamp, const FVector& InParryDirection)
				: FAction(Timestamp), ParryDirection(InParryDirection) { }
							
			virtual bool IsImmediate() const override { return true; }
			virtual bool CanCombine(FAction* OtherAction) const override { return false; }
			virtual FName GetActionName() const override { return LAActionKeys::Key_Action_Combat_Parry; }
			virtual FLearningAgentsActionObjectElement GetAction(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
							
		private:
			FVector ParryDirection = FVector::ZeroVector;
	};
	
	class FAction_Dodge : public FAction
	{
		public:
			FAction_Dodge(float Timestamp, const FVector& InDodgeDirection)
				: FAction(Timestamp), DodgeDirection(InDodgeDirection) { }
								
			virtual bool IsImmediate() const override { return true; }
			virtual bool CanCombine(FAction* OtherAction) const override { return false; }
			virtual FName GetActionName() const override { return LAActionKeys::Key_Action_Combat_Dodge; }
			virtual FLearningAgentsActionObjectElement GetAction(ULearningAgentsActionObject* InActionObject, AActor* AgentActor) const override;
								
		private:
			FVector DodgeDirection = FVector::ZeroVector;
	};

	struct FAgentPendingActions
	{
		DECLARE_DELEGATE_OneParam(FActionAccumulatedEvent, int AgentId);
		
		FAgentPendingActions(int AgentId);
		~FAgentPendingActions();
		
		void AddAction(const TSharedPtr<FAction>& NewAction);
		TSharedPtr<FAction> GetAction();
	
		FActionAccumulatedEvent ActionAccumulatedEvent;
		
	private:
		// this one is the default action to accumulate to
		TSharedPtr<FAction> AccumulatingAction;
		
		// this is the action where a new action is stored if the AccumulatingAction can't combine new action, which also means that the accumulating action is full 
		TSharedPtr<FAction> PendingAction;
		
		int AgentId = -1;
	};
}
