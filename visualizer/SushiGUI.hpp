# pragma once
# include <Siv3D.hpp>

namespace s3d {
	namespace SushiGUI {

		using position_type = Vec2;
		using size_type = Vec2;

		void draw_button_label(const String& label, const Rect& rect, const Font& font, const Color& color);

		constexpr double rect_stretch_rate = 20;
		void draw_button_label(const StringView& label, const RectF& rectf, const Font& font, const Color& color);
		bool is_click_button(const RectF& rectf);
		bool is_click_button(const RoundRect& roundrect);


		const Color button1_color_release{ U"#EA4C89" };
		const Color button1_color_mouseover{ U"#F082AC" };
		const Color button1_color_label{ U"#FFFFFF" };
		const double button1_roundrect_rate = 5;
		bool button1(const Font& font, const StringView& label, const Arg::topLeft_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button1(const Font& font, const StringView& label, const Arg::topCenter_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button1(const Font& font, const StringView& label, const Arg::topRight_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button1(const Font& font, const StringView& label, const Arg::leftCenter_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button1(const Font& font, const StringView& label, const Arg::center_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button1(const Font& font, const StringView& label, const Arg::rightCenter_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button1(const Font& font, const StringView& label, const Arg::bottomLeft_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button1(const Font& font, const StringView& label, const Arg::bottomCenter_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button1(const Font& font, const StringView& label, const Arg::bottomRight_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button1(const Font& font, const StringView& label, const position_type& pos, const size_type& size, bool enabled = true);

		constexpr Color button2_color{ U"#F5F5F5" };
		constexpr Color button2_color_label{ U"#333333" };
		constexpr double button2_roundrect_rate = 5;
		bool button2(const Font& font, const StringView& label, const Arg::topLeft_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button2(const Font& font, const StringView& label, const Arg::topCenter_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button2(const Font& font, const StringView& label, const Arg::topRight_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button2(const Font& font, const StringView& label, const Arg::leftCenter_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button2(const Font& font, const StringView& label, const Arg::center_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button2(const Font& font, const StringView& label, const Arg::rightCenter_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button2(const Font& font, const StringView& label, const Arg::bottomLeft_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button2(const Font& font, const StringView& label, const Arg::bottomCenter_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button2(const Font& font, const StringView& label, const Arg::bottomRight_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button2(const Font& font, const StringView& label, const position_type& pos, const size_type& size, bool enabled = true);

		constexpr Color button3_color_release{ U"#2EA44F" };
		constexpr Color button3_color_mouseover(U"#2C974B");
		constexpr Color button3_color_press(U"#298E46");
		constexpr Color button3_color_label(U"#FFFFFF");
		constexpr double button3_roundrect_rate = 5;
		bool button3(const Font& font, const StringView& label, const Arg::topLeft_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button3(const Font& font, const StringView& label, const Arg::topCenter_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button3(const Font& font, const StringView& label, const Arg::topRight_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button3(const Font& font, const StringView& label, const Arg::leftCenter_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button3(const Font& font, const StringView& label, const Arg::center_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button3(const Font& font, const StringView& label, const Arg::rightCenter_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button3(const Font& font, const StringView& label, const Arg::bottomLeft_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button3(const Font& font, const StringView& label, const Arg::bottomCenter_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button3(const Font& font, const StringView& label, const Arg::bottomRight_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button3(const Font& font, const StringView& label, const position_type& pos, const size_type& size, bool enabled = true);

		constexpr Color button4_color_release{ U"#F8F9FA" };
		constexpr Color button4_color_mouseover(U"#F1F2F4");
		constexpr Color button4_color_press(U"#EBEDF0");
		constexpr Color button4_color_frame(U"#D6D8D8");
		constexpr Color button4_color_label(U"#24292E");
		constexpr double button4_roundrect_rate = 5;
		bool button4(const Font& font, const StringView& label, const Arg::topLeft_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button4(const Font& font, const StringView& label, const Arg::topCenter_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button4(const Font& font, const StringView& label, const Arg::topRight_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button4(const Font& font, const StringView& label, const Arg::leftCenter_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button4(const Font& font, const StringView& label, const Arg::center_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button4(const Font& font, const StringView& label, const Arg::rightCenter_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button4(const Font& font, const StringView& label, const Arg::bottomLeft_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button4(const Font& font, const StringView& label, const Arg::bottomCenter_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button4(const Font& font, const StringView& label, const Arg::bottomRight_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button4(const Font& font, const StringView& label, const position_type& pos, const size_type& size, bool enabled = true);

		bool button5(const Font& font, const StringView& label, const Arg::topLeft_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button5(const Font& font, const StringView& label, const Arg::topCenter_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button5(const Font& font, const StringView& label, const Arg::topRight_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button5(const Font& font, const StringView& label, const Arg::leftCenter_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button5(const Font& font, const StringView& label, const Arg::center_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button5(const Font& font, const StringView& label, const Arg::rightCenter_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button5(const Font& font, const StringView& label, const Arg::bottomLeft_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button5(const Font& font, const StringView& label, const Arg::bottomCenter_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button5(const Font& font, const StringView& label, const Arg::bottomRight_<position_type>& anchor, const size_type& size, bool enabled = true);
		bool button5(const Font& font, const StringView& label, const position_type& pos, const size_type& size, bool enabled = true);



	}
}
