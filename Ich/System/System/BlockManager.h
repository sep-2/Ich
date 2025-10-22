#pragma once

#include <Siv3D.hpp>
#include <utility>

/// <summary>
/// 縺ｲ繧峨′縺ｪ繝悶Ο繝・け縺ｮ髮・粋繧偵ｂ縺ｨ縺ｫ縲∬ｾ樊嶌蜀・・蜊倩ｪ槭′謌千ｫ九☆繧九°縺ｩ縺・°繧貞愛螳壹☆繧九◆繧√・繝ｦ繝ｼ繝・ぅ繝ｪ繝・ぅ縲・/// 繧ｲ繝ｼ繝荳ｭ縺ｧ縺ｯ縲∵焔蜈・・繝悶Ο繝・け縺ｨ霎樊嶌繧呈ｸ｡縺吶□縺代〒繝偵ャ繝医・繝ｪ繝ｼ繝√・謚ｽ蜃ｺ縺瑚｡後∴繧九ｈ縺・↓縺吶ｋ縲・/// </summary>
class BlockManager
{
public:
  BlockManager();
  ~BlockManager();

  /// <summary>
  /// 繝悶Ο繝・け縺縺代〒螳悟・縺ｫ邨・∩遶九※繧峨ｌ繧具ｼ・繝偵ャ繝医☆繧具ｼ牙腰隱槭ｒ謚ｽ蜃ｺ縺吶ｋ縲・  /// 繝悶Ο繝・け縺ｨ霎樊嶌蜀・・蜊倩ｪ槭・縲∵ｿ∫せ繝ｻ蜊頑ｿ∫せ繝ｻ蟆乗枚蟄励ｒ蛹ｺ蛻･縺帙★縺ｫ遯√″蜷医ｏ縺帙ｋ縲・  /// </summary>
  /// <param name="blocks">迴ｾ蝨ｨ菫晄戟縺励※縺・ｋ繝悶Ο繝・け荳隕ｧ縲・隕∫ｴ縺ｫ縺､縺・譁・ｭ励ｒ諠ｳ螳壹・/param>
  /// <param name="dictionary">蛻､螳壼ｯｾ雎｡縺ｨ縺ｪ繧句腰隱槭・荳隕ｧ縲・/param>
  /// <returns>繝偵ャ繝医＠縺溷腰隱槭ｒ霎樊嶌鬆・・縺ｾ縺ｾ霑斐☆縲・/returns>
  Array<String> GetHitWords(const Array<String>& blocks, const Array<String>& dictionary) const;

  /// <summary>
  /// 繝悶Ο繝・け縺ｫ縺ゅ→1譁・ｭ怜刈縺医ｋ縺縺代〒螳梧・縺吶ｋ・・繝ｪ繝ｼ繝∫憾諷九・・牙腰隱槭ｒ謚ｽ蜃ｺ縺吶ｋ縲・  /// 雜ｳ繧翫↑縺・枚蟄励・豁｣隕丞喧蠕後〒縺ｯ縺ｪ縺上瑚ｾ樊嶌縺ｫ險倩ｼ峨＆繧後※縺・ｋ蜈・・譁・ｭ励阪ｒ霑泌唆縺吶ｋ縲・  /// </summary>
  /// <param name="blocks">迴ｾ蝨ｨ菫晄戟縺励※縺・ｋ繝悶Ο繝・け荳隕ｧ縲・/param>
  /// <param name="dictionary">蛻､螳壼ｯｾ雎｡縺ｮ蜊倩ｪ樔ｸ隕ｧ縲・/param>
  /// <returns>
  /// first: 蜊倩ｪ槭◎縺ｮ繧ゅ・ / second: 雜ｳ繧翫↑縺・枚蟄暦ｼ郁ｾ樊嶌縺ｮ陦ｨ險倥↓蜷医ｏ縺帙◆1譁・ｭ暦ｼ峨・  /// </returns>
  Array<std::pair<String, String>> GetReachWords(const Array<String>& blocks, const Array<String>& dictionary) const;

  /// <summary>
  /// 霎樊嶌縺ｮ蜊倩ｪ樒ｾ､縺九ｉ繝ｩ繝ｳ繝繝縺ｫ譁・ｭ励ｒ謚懊″蜃ｺ縺励〉ow ﾃ・column 縺ｮ繝悶Ο繝・け驟咲ｽｮ繧堤函謌舌☆繧九・  /// batchSize 譁・ｭ怜・繧堤岼螳峨↓霎樊嶌縺九ｉ譁・ｭ励ｒ遒ｺ菫昴＠縲∽ｺ梧ｬ｡蜈・・蛻励↓謨ｴ蠖｢縺励※霑斐☆縲・  /// row ﾃ・column 縺・batchSize 縺ｮ蛟肴焚縺ｧ縺ｪ縺・ｴ蜷医・ std::invalid_argument 繧帝∝・縺吶ｋ縲・  /// </summary>
  /// <param name="row">逕滓・縺吶ｋ陦梧焚縲・ 莉･荳翫〒縺ゅｋ縺薙→縲・/param>
  /// <param name="column">逕滓・縺吶ｋ蛻玲焚縲・ 莉･荳翫〒縺ゅｋ縺薙→縲・/param>
  /// <param name="batchSize">蛟呵｣懈歓蜃ｺ譎ゅ↓遒ｺ菫昴＠縺溘＞譛蟆乗枚蟄玲焚縲りｾ樊嶌隱槭ｒ鬆・↓遨阪∩荳翫￡縺ｦ蛻ｰ驕斐＆縺帙ｋ縲・/param>
  /// <param name="dictionary">繝悶Ο繝・け蛟呵｣懊→縺励※蛻ｩ逕ｨ縺吶ｋ蜊倩ｪ樔ｸ隕ｧ縲らｩｺ縺ｮ蝣ｴ蜷医・遨ｺ驟榊・繧定ｿ斐☆縲・/param>
  /// <returns>row ﾃ・column 縺ｮ繧ｵ繧､繧ｺ繧呈戟縺､繝悶Ο繝・け驟榊・縲よ擅莉ｶ繧呈ｺ縺溘○縺ｪ縺・ｴ蜷医・遨ｺ驟榊・縲・/returns>
  Array<Array<String>> GenerateBlockGrid(int32 row, int32 column, int32 batchSize, const Array<String>& dictionary) const;
};
