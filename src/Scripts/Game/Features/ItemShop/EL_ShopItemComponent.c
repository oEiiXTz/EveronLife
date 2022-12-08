[ComponentEditorProps(category: "EveronLife/ShopItem", description: "Shop Item Reference")]
class EL_ShopItemComponentClass : ScriptComponentClass
{
}

class EL_ShopItemComponent : ScriptComponent
{
	[Attribute("{9CA1D6CE3F6914BA}Configs/ItemShop/EL_Shops.conf", UIWidgets.Object, "Shop config")]
	protected ref EL_ShopsConfig m_ShopsConfig;

	[Attribute("", UIWidgets.ResourcePickerThumbnail, "", "et")]
	protected ResourceName m_ShopItemPrefab;

	protected IEntity m_ShopItemEntity;
	protected EL_Price m_ShopItemPriceConfig;

	//------------------------------------------------------------------------------------------------
	EL_Price GetShopItemPriceConfig()
	{
		//Not init yet
		if (!m_ShopItemPriceConfig)
			m_ShopItemPriceConfig = FindPrefabShopItemConfig();
		return m_ShopItemPriceConfig;
	}

	//------------------------------------------------------------------------------------------------
	ResourceName GetShopItemPrefab()
	{
		return m_ShopItemPrefab;
	}

	//------------------------------------------------------------------------------------------------
	IEntity GetShopItemEntity()
	{
		if (!m_ShopItemEntity)
			m_ShopItemEntity = GetGame().SpawnEntityPrefab(Resource.Load(m_ShopItemPrefab));
		return m_ShopItemEntity;
	}

	//------------------------------------------------------------------------------------------------
	protected EL_Price FindPrefabShopItemConfig()
	{
		if (!m_ShopsConfig || !m_ShopItemPrefab)
			return null;
		
		foreach (EL_ShopConfig shopConfig : m_ShopsConfig.m_aShopConfigs)
		{
			foreach (EL_Price price : shopConfig.m_aPriceConfigs)
			{
				if (price.m_Prefab == m_ShopItemPrefab)
				{
					return price;
				}
			}
		}
		return null;
	}

	//------------------------------------------------------------------------------------------------
	//! Set Mesh to shop item mesh if not already set. This allows buying/selling from other meshes (eg. Apples from apple crate)
	override void EOnInit(IEntity owner)
	{
		if (!m_ShopItemPrefab)
			return;
		
		//Create Mesh
		if (!owner.GetVObject())
		{
			owner.SetObject(EL_Utils.GetPrefabVObject(m_ShopItemPrefab), "");
		}
		
		//Create Hitbox
		Physics phys = Physics.CreateStatic(owner, -1);
		phys.SetInteractionLayer(EPhysicsLayerPresets.FireGeo);
		owner.SetFlags(EntityFlags.ACTIVE, false);
		
		
		m_ShopItemPriceConfig = FindPrefabShopItemConfig();
		if (!m_ShopItemPriceConfig)
			Print("No price config found for prefab: " + m_ShopItemPrefab ,LogLevel.WARNING);
		
		m_ShopItemEntity = GetGame().SpawnEntityPrefab(Resource.Load(m_ShopItemPrefab));
	}

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
		owner.SetFlags(EntityFlags.ACTIVE, false);
	}
}