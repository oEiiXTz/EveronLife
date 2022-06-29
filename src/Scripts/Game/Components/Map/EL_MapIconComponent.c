class EL_MapIconComponentClass : ScriptComponentClass
{
}

class EL_MapIconComponent : ScriptComponent
{
	[Attribute("", UIWidgets.ResourcePickerThumbnail, "Icon Image", params: "edds imageset")]
	private ResourceName m_ImagePath;
	
	[Attribute("", UIWidgets.EditBox, "Image name if Image is imageset")]
	private string m_sImageSetName;
	
	[Attribute("1", UIWidgets.Auto, "")]
	private float m_fMinScale;	
	[Attribute("1", UIWidgets.Auto, "")]
	private float m_fMaxScale;
	
	private SCR_MapEntity m_MapEntity;
	private Widget m_wIconFrame;
	private ImageWidget m_wIconWidget
	private vector m_vBaseIconSize;
	//------------------------------------------------------------------------------------------------
	private void UpdateIcon()
	{
		if (!m_MapEntity)
			return;
		

		vector entPos = GetOwner().GetOrigin();
		float x, y;
		m_MapEntity.WorldToScreen(entPos[0], entPos[2], x, y, true);

		x = GetGame().GetWorkspace().DPIUnscale(x);
		y = GetGame().GetWorkspace().DPIUnscale(y);
		
		
		FrameSlot.SetPos(m_wIconFrame, x, y);
	}
	
	//------------------------------------------------------------------------------------------------
	private void OnMapOpen()
	{
		m_MapEntity = SCR_MapEntity.GetMapInstance();
		if (!m_MapEntity)
			return;
		
		Widget mapWidget = m_MapEntity.GetMapConfig().RootWidgetRef;
		if (!mapWidget)
			return;
		
		m_wIconFrame = GetGame().GetWorkspace().CreateWidgets("{E7F2D3A11AD40D97}UI/Layouts/Map/El_IconLayout.layout", mapWidget);
		
		//Set icon image
		m_wIconWidget = ImageWidget.Cast(m_wIconFrame.FindAnyWidget("Icon"));

		if (m_wIconWidget)
		{
			if (m_ImagePath.EndsWith("imageset") && !m_sImageSetName.IsEmpty())
			{
				m_wIconWidget.LoadImageFromSet(0, m_ImagePath, m_sImageSetName);
			}
			else
			{
				m_wIconWidget.LoadImageTexture(0, m_ImagePath);
			}

			m_vBaseIconSize = m_wIconWidget.GetSize();
		}
		
		//Update postion to match pan and zoom
		UpdateIcon();
		OnLayerChanged();
		SCR_MapEntity.GetOnMapPan().Insert(UpdateIcon);
		SCR_MapEntity.GetOnMapZoom().Insert(UpdateIcon);
	}
	
	//------------------------------------------------------------------------------------------------
	private void OnMapClose()
	{
		SCR_MapEntity.GetOnMapPan().Remove(UpdateIcon);
		SCR_MapEntity.GetOnMapZoom().Remove(UpdateIcon);
	}	
	
	//------------------------------------------------------------------------------------------------
	private void OnLayerChanged()
	{
		if (!m_MapEntity)
			return;
		int curLayerIndex = m_MapEntity.GetLayerIndex();
		float layerScale = curLayerIndex * 0.33;
		layerScale = Math.Clamp(layerScale, m_fMinScale, m_fMaxScale);
		m_wIconWidget.SetSize(m_vBaseIconSize[0] * layerScale, m_vBaseIconSize[1] * layerScale);
	}
	
	//------------------------------------------------------------------------------------------------
	void EL_MapIconComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		SCR_MapEntity.GetOnMapOpen().Insert(OnMapOpen);
		SCR_MapEntity.GetOnMapClose().Insert(OnMapClose);
		SCR_MapEntity.GetOnLayerChanged().Insert(OnLayerChanged);
	}
	
	//------------------------------------------------------------------------------------------------
	void ~EL_MapIconComponent()
	{
		SCR_MapEntity.GetOnMapOpen().Remove(OnMapOpen);
		SCR_MapEntity.GetOnMapClose().Remove(OnMapClose);
		SCR_MapEntity.GetOnLayerChanged().Remove(OnLayerChanged);
	}


};
