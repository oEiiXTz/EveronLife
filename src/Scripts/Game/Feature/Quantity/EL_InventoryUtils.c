modded class EL_InventoryUtils
{
	//------------------------------------------------------------------------------------------------
	//! Returns the total amount or -1 on faulty opertations.
	static int GetAmount(InventoryStorageManagerComponent target, ResourceName prefab)
	{
		if (!target) return -1;

		// No quantity used, so default engine count function
		if (!SCR_BaseContainerTools.FindComponentSource(Resource.Load(prefab), EL_QuantityComponent))
		{
			return target.GetDepositItemCountByResource(prefab);
		}

		int totalAmount;
		foreach (IEntity item : FindItemsByPrefab(target, prefab))
		{
			int amount = 1;
			EL_QuantityComponent quantity = EL_ComponentFinder<EL_QuantityComponent>.Find(item);
			if (quantity) amount = quantity.GetQuantity();
			totalAmount += amount;
		}

		return totalAmount;
	}

	//------------------------------------------------------------------------------------------------
	//! \ref GetAmount(InventoryStorageManagerComponent, ResourceName)
	static int GetAmount(IEntity target, ResourceName prefab)
	{
		InventoryStorageManagerComponent invManager = EL_InventoryUtils.GetResponsibleStorageManager(target);
		return GetAmount(invManager, prefab);
	}

	//------------------------------------------------------------------------------------------------
	//! Returns acutal amount added or 0 on faulty opertations.
	static int AddAmount(InventoryStorageManagerComponent storageManager, ResourceName prefab, int amount, bool dropOverflow = false)
	{
		if (!storageManager || amount <= 0) return 0;

		int remainainAmount = amount;

		bool isQuantityPrefab = SCR_BaseContainerTools.FindComponentSource(Resource.Load(prefab), EL_QuantityComponent);
		if (isQuantityPrefab)
		{
			// Try fill up existing quantity components first
			array<IEntity> prefabItems = FindItemsByPrefab(storageManager, prefab);
			array<EL_QuantityComponent> quantityComponents = EL_QuantityComponent.ExtractQuantityComponents(prefabItems);
			array<EL_QuantityComponent> sortedQuantityComponents = EL_QuantityComponent.SortByQuantity(quantityComponents);
			foreach (EL_QuantityComponent quantityComponent : sortedQuantityComponents)
			{
				int added;
				quantityComponent.AddQuantity(remainainAmount, true, added);
				remainainAmount -= added;
				if (remainainAmount <= 0) return amount;
			}
		}

		while (remainainAmount > 0)
		{
			IEntity item = EL_Utils.SpawnEntityPrefab(prefab, storageManager.GetOwner().GetOrigin());
			if (!item || (!storageManager.TryInsertItem(item) && !dropOverflow))
			{
				SCR_EntityHelper.DeleteEntityAndChildren(item);
				break;
			}

			if (isQuantityPrefab && remainainAmount > 1)
			{
				EL_QuantityComponent quantityComponent = EL_ComponentFinder<EL_QuantityComponent>.Find(item);
				int added;
				quantityComponent.AddQuantity(remainainAmount - 1, true, added); // One quantity added through existence of the new item
				remainainAmount -= added + 1;
			}
			else
			{
				remainainAmount--;
			}
		}

		return amount - remainainAmount;
	}

	//------------------------------------------------------------------------------------------------
	//! \ref AddAmount(InventoryStorageManagerComponent, ResourceName, int, bool)
	static int AddAmount(IEntity target, ResourceName prefab, int amount, bool dropOverflow = false)
	{
		InventoryStorageManagerComponent invManager = EL_InventoryUtils.GetResponsibleStorageManager(target);
		return AddAmount(invManager, prefab, amount, dropOverflow);
	}

	//------------------------------------------------------------------------------------------------
	//! Returns acutal amount removed or 0 on faulty opertations.
	static int RemoveAmount(InventoryStorageManagerComponent storageManager, ResourceName prefab, int amount)
	{
		if (!storageManager || amount <= 0) return 0;

		int remainainAmount = amount;

		array<IEntity> prefabItems = FindItemsByPrefab(storageManager, prefab);

		if (SCR_BaseContainerTools.FindComponentSource(Resource.Load(prefab), EL_QuantityComponent))
		{
			// Remove lowest quantity first
			array<EL_QuantityComponent> quantityComponents = EL_QuantityComponent.ExtractQuantityComponents(prefabItems);
			array<EL_QuantityComponent> sortedQuantityComponents = EL_QuantityComponent.SortByQuantity(quantityComponents, false);
			foreach (EL_QuantityComponent quantityComponent : sortedQuantityComponents)
			{
				int quantityChange;
				quantityComponent.AddQuantity(-remainainAmount, true, quantityChange);
				remainainAmount += quantityChange;
				if (remainainAmount <= 0) return amount;
			}
		}
		else
		{
			foreach (IEntity item : prefabItems)
			{
				SCR_EntityHelper.DeleteEntityAndChildren(item);
				if (--remainainAmount <= 0) return amount;
			}
		}

		return amount - remainainAmount;
	}
	
	//------------------------------------------------------------------------------------------------
	//! \ref RemoveAmount(InventoryStorageManagerComponent, ResourceName, int)
	static int RemoveAmount(IEntity target, ResourceName prefab, int amount)
	{
		InventoryStorageManagerComponent invManager = EL_InventoryUtils.GetResponsibleStorageManager(target);
		return RemoveAmount(invManager, prefab, amount);
	}
}