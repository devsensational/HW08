#pragma once

#include "CoreMinimal.h"
#include "MGLevelRow.generated.h"

USTRUCT(BlueprintType)
struct FMGLevelRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FName LevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	int32 MaxItemCount;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	float LevelDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	TObjectPtr<UDataTable> ItemDataTable;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FString LevelDescription;
};

