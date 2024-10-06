# pragma once
# include <Siv3D.hpp>
# include "SushiGUI.hpp"

namespace s3d {
	namespace SushiGUI {

		void draw_button_label(const StringView& label, const RectF& rectf, const Font& font, const Color& color) {
			int left = 0, right = 1000;
			while (Abs(right - left) > 1) {
				int mid = (left + right) / 2;
				if (font(label).draw(mid, rectf.stretched(Min(-rectf.h / rect_stretch_rate, -rectf.w / rect_stretch_rate)), HSV{ 0,0 })) {
					left = mid;
				}else {
					right = mid;
				}
			}
			font(label).drawAt(left, rectf.center(), color);
		}
		bool is_click_button(const RectF& rectf) {
			if (rectf.mouseOver()) {
				Cursor::RequestStyle(CursorStyle::Hand);
			}
			return rectf.leftClicked();
		}
		bool is_click_button(const RoundRect& roundrect) {
			if (roundrect.mouseOver()) {
				Cursor::RequestStyle(CursorStyle::Hand);
			}
			return roundrect.leftClicked();
		}
		
		bool button1(const Font& font, const StringView& label, const Arg::topLeft_<position_type>& anchor, const size_type& size, bool enabled) {
			return button1(font, label, position_type{ anchor->x, anchor->y }, size, enabled);
		}
		bool button1(const Font& font, const StringView& label, const Arg::topCenter_<position_type>& anchor, const size_type& size, bool enabled) {
			return button1(font, label, position_type{ anchor->x - size.x / 2, anchor->y }, size, enabled);
		}
		bool button1(const Font& font, const StringView& label, const Arg::topRight_<position_type>& anchor, const size_type& size, bool enabled) {
			return button1(font, label, position_type{ anchor->x - size.x, anchor->y }, size, enabled);
		}
		bool button1(const Font& font, const StringView& label, const Arg::leftCenter_<position_type>& anchor, const size_type& size, bool enabled) {
			return button1(font, label, position_type{ anchor->x, anchor->y - size.y / 2 }, size, enabled);
		}
		bool button1(const Font& font, const StringView& label, const Arg::center_<position_type>& anchor, const size_type& size, bool enabled) {
			return button1(font, label, position_type{ anchor->x - size.x / 2, anchor->y - size.y / 2 }, size, enabled);
		}
		bool button1(const Font& font, const StringView& label, const Arg::rightCenter_<position_type>& anchor, const size_type& size, bool enabled) {
			return button1(font, label, position_type{ anchor->x - size.x, anchor->y - size.y / 2 }, size, enabled);
		}
		bool button1(const Font& font, const StringView& label, const Arg::bottomLeft_<position_type>& anchor, const size_type& size, bool enabled) {
			return button1(font, label, position_type{ anchor->x, anchor->y - size.y }, size, enabled);
		}
		bool button1(const Font& font, const StringView& label, const Arg::bottomCenter_<position_type>& anchor, const size_type& size, bool enabled) {
			return button1(font, label, position_type{ anchor->x / 2, anchor->y - size.y }, size, enabled);
		}
		bool button1(const Font& font, const StringView& label, const Arg::bottomRight_<position_type>& anchor, const size_type& size, bool enabled) {
			return button1(font, label, position_type{ anchor->x - size.x, anchor->y - size.y }, size, enabled);
		}
		bool button1(const Font& font, const StringView& label, const position_type& pos, const size_type& size, bool enabled) {
			Console << enabled;
			const RectF button_rectf{ pos, size };
			const RoundRect button_roundrect = button_rectf.rounded(Min(size.x / button1_roundrect_rate, size.y / button1_roundrect_rate));
			const ScopedColorMul2D colorMul{ enabled ? ColorF{ 1.0 } : ColorF{ 0.5 } };
			 if (enabled and button_roundrect.mouseOver()) button_roundrect.draw(button1_color_mouseover);
			else button_roundrect.draw(button1_color_release);
			draw_button_label(label, button_rectf, font, button1_color_label);
			return (enabled and is_click_button(button_roundrect));
		}


		bool button2(const Font& font, const StringView& label, const Arg::topLeft_<position_type>& anchor, const size_type& size, bool enabled) {
			return button2(font, label, position_type{ anchor->x, anchor->y }, size, enabled);
		}
		bool button2(const Font& font, const StringView& label, const Arg::topCenter_<position_type>& anchor, const size_type& size, bool enabled) {
			return button2(font, label, position_type{ anchor->x - size.x / 2, anchor->y }, size, enabled);
		}
		bool button2(const Font& font, const StringView& label, const Arg::topRight_<position_type>& anchor, const size_type& size, bool enabled) {
			return button2(font, label, position_type{ anchor->x - size.x, anchor->y }, size, enabled);
		}
		bool button2(const Font& font, const StringView& label, const Arg::leftCenter_<position_type>& anchor, const size_type& size, bool enabled) {
			return button2(font, label, position_type{ anchor->x, anchor->y - size.y / 2 }, size, enabled);
		}
		bool button2(const Font& font, const StringView& label, const Arg::center_<position_type>& anchor, const size_type& size, bool enabled) {
			return button2(font, label, position_type{ anchor->x - size.x / 2, anchor->y - size.y / 2 }, size, enabled);
		}
		bool button2(const Font& font, const StringView& label, const Arg::rightCenter_<position_type>& anchor, const size_type& size, bool enabled) {
			return button2(font, label, position_type{ anchor->x - size.x, anchor->y - size.y / 2 }, size, enabled);
		}
		bool button2(const Font& font, const StringView& label, const Arg::bottomLeft_<position_type>& anchor, const size_type& size, bool enabled) {
			return button2(font, label, position_type{ anchor->x, anchor->y - size.y }, size, enabled);
		}
		bool button2(const Font& font, const StringView& label, const Arg::bottomCenter_<position_type>& anchor, const size_type& size, bool enabled) {
			return button2(font, label, position_type{ anchor->x / 2, anchor->y - size.y }, size, enabled);
		}
		bool button2(const Font& font, const StringView& label, const Arg::bottomRight_<position_type>& anchor, const size_type& size, bool enabled) {
			return button2(font, label, position_type{ anchor->x - size.x, anchor->y - size.y }, size, enabled);
		}
		bool button2(const Font& font, const StringView& label, const position_type& pos, const size_type& size, bool enabled) {
			const RectF button_rectf{ pos, size };
			const RoundRect button_roundrect = button_rectf.rounded(Min(size.x / button2_roundrect_rate, size.y / button2_roundrect_rate));
			const ScopedColorMul2D colorMul{ enabled ? ColorF{ 1.0 } : ColorF{ 0.5 } };
			button_roundrect.draw(button2_color);
			draw_button_label(label, button_rectf, font, button2_color_label);
			return (enabled and is_click_button(button_roundrect));
		}


		bool button3(const Font& font, const StringView& label, const Arg::topLeft_<position_type>& anchor, const size_type& size, bool enabled) {
			return button3(font, label, position_type{ anchor->x, anchor->y }, size, enabled);
		}
		bool button3(const Font& font, const StringView& label, const Arg::topCenter_<position_type>& anchor, const size_type& size, bool enabled) {
			return button3(font, label, position_type{ anchor->x - size.x / 2, anchor->y }, size, enabled);
		}
		bool button3(const Font& font, const StringView& label, const Arg::topRight_<position_type>& anchor, const size_type& size, bool enabled) {
			return button3(font, label, position_type{ anchor->x - size.x, anchor->y }, size, enabled);
		}
		bool button3(const Font& font, const StringView& label, const Arg::leftCenter_<position_type>& anchor, const size_type& size, bool enabled) {
			return button3(font, label, position_type{ anchor->x, anchor->y - size.y / 2 }, size, enabled);
		}
		bool button3(const Font& font, const StringView& label, const Arg::center_<position_type>& anchor, const size_type& size, bool enabled) {
			return button3(font, label, position_type{ anchor->x - size.x / 2, anchor->y - size.y / 2 }, size, enabled);
		}
		bool button3(const Font& font, const StringView& label, const Arg::rightCenter_<position_type>& anchor, const size_type& size, bool enabled) {
			return button3(font, label, position_type{ anchor->x - size.x, anchor->y - size.y / 2 }, size, enabled);
		}
		bool button3(const Font& font, const StringView& label, const Arg::bottomLeft_<position_type>& anchor, const size_type& size, bool enabled) {
			return button3(font, label, position_type{ anchor->x, anchor->y - size.y }, size, enabled);
		}
		bool button3(const Font& font, const StringView& label, const Arg::bottomCenter_<position_type>& anchor, const size_type& size, bool enabled) {
			return button3(font, label, position_type{ anchor->x / 2, anchor->y - size.y }, size, enabled);
		}
		bool button3(const Font& font, const StringView& label, const Arg::bottomRight_<position_type>& anchor, const size_type& size, bool enabled) {
			return button3(font, label, position_type{ anchor->x - size.x, anchor->y - size.y }, size, enabled);
		}
		bool button3(const Font& font, const StringView& label, const position_type& pos, const size_type& size, bool enabled) {
			const RectF button_rectf{ pos, size };
			const RoundRect button_roundrect = button_rectf.rounded(Min(size.x, size.y)/button3_roundrect_rate);
			const ScopedColorMul2D colorMul{ enabled ? ColorF{ 1.0 } : ColorF{ 0.5 } };
			if (enabled and button_roundrect.leftPressed()) button_roundrect.draw(button3_color_press);
			else if (enabled and button_roundrect.mouseOver()) button_roundrect.draw(button3_color_mouseover);
			else button_roundrect.draw(button3_color_release);
			draw_button_label(label, button_rectf, font, button3_color_label);
			return (enabled and is_click_button(button_roundrect));
		}


		bool button4(const Font& font, const StringView& label, const Arg::topLeft_<position_type>& anchor, const size_type& size, bool enabled) {
			return button4(font, label, position_type{ anchor->x, anchor->y }, size, enabled);
		}
		bool button4(const Font& font, const StringView& label, const Arg::topCenter_<position_type>& anchor, const size_type& size, bool enabled) {
			return button4(font, label, position_type{ anchor->x - size.x / 2, anchor->y }, size, enabled);
		}
		bool button4(const Font& font, const StringView& label, const Arg::topRight_<position_type>& anchor, const size_type& size, bool enabled) {
			return button4(font, label, position_type{ anchor->x - size.x, anchor->y }, size, enabled);
		}
		bool button4(const Font& font, const StringView& label, const Arg::leftCenter_<position_type>& anchor, const size_type& size, bool enabled) {
			return button4(font, label, position_type{ anchor->x, anchor->y - size.y / 2 }, size, enabled);
		}
		bool button4(const Font& font, const StringView& label, const Arg::center_<position_type>& anchor, const size_type& size, bool enabled) {
			return button4(font, label, position_type{ anchor->x - size.x / 2, anchor->y - size.y / 2 }, size, enabled);
		}
		bool button4(const Font& font, const StringView& label, const Arg::rightCenter_<position_type>& anchor, const size_type& size, bool enabled) {
			return button4(font, label, position_type{ anchor->x - size.x, anchor->y - size.y / 2 }, size, enabled);
		}
		bool button4(const Font& font, const StringView& label, const Arg::bottomLeft_<position_type>& anchor, const size_type& size, bool enabled) {
			return button4(font, label, position_type{ anchor->x, anchor->y - size.y }, size, enabled);
		}
		bool button4(const Font& font, const StringView& label, const Arg::bottomCenter_<position_type>& anchor, const size_type& size, bool enabled) {
			return button4(font, label, position_type{ anchor->x / 2, anchor->y - size.y }, size, enabled);
		}
		bool button4(const Font& font, const StringView& label, const Arg::bottomRight_<position_type>& anchor, const size_type& size, bool enabled) {
			return button4(font, label, position_type{ anchor->x - size.x, anchor->y - size.y }, size, enabled);
		}
		bool button4(const Font& font, const StringView& label, const position_type& pos, const size_type& size, bool enabled) {
			const RectF button_rectf{ pos, size };
			const RoundRect button_roundrect = button_rectf.rounded(Min(size.x / button4_roundrect_rate, size.y / button4_roundrect_rate));
			const ScopedColorMul2D colorMul{ enabled ? ColorF{ 1.0 } : ColorF{ 0.5 } };
			if (enabled and button_roundrect.leftPressed()) button_roundrect.draw(button4_color_press);
			else if (enabled and button_roundrect.mouseOver()) button_roundrect.draw(button4_color_mouseover);
			else button_roundrect.draw(button4_color_release);
			button_roundrect.drawFrame(Min(size.x, size.y)/50.0, button4_color_frame);
			draw_button_label(label, button_rectf, font, button4_color_label);
			return (enabled and is_click_button(button_roundrect));
		}


		//bool button5(const Font& font, const StringView& label, const Arg::topLeft_<position_type>& anchor, const size_type& size) {
		//	return button5(font, label, position_type{ anchor->x, anchor->y }, size);
		//}
		//bool button5(const Font& font, const StringView& label, const Arg::topCenter_<position_type>& anchor, const size_type& size) {
		//	return button5(font, label, position_type{ anchor->x - size.x / 2, anchor->y }, size);
		//}
		//bool button5(const Font& font, const StringView& label, const Arg::topRight_<position_type>& anchor, const size_type& size) {
		//	return button5(font, label, position_type{ anchor->x - size.x, anchor->y }, size);
		//}
		//bool button5(const Font& font, const StringView& label, const Arg::leftCenter_<position_type>& anchor, const size_type& size) {
		//	return button5(font, label, position_type{ anchor->x, anchor->y - size.y / 2 }, size);
		//}
		//bool button5(const Font& font, const StringView& label, const Arg::center_<position_type>& anchor, const size_type& size) {
		//	return button5(font, label, position_type{ anchor->x - size.x / 2, anchor->y - size.y / 2 }, size);
		//}
		//bool button5(const Font& font, const StringView& label, const Arg::rightCenter_<position_type>& anchor, const size_type& size) {
		//	return button5(font, label, position_type{ anchor->x - size.x, anchor->y - size.y / 2 }, size);
		//}
		//bool button5(const Font& font, const StringView& label, const Arg::bottomLeft_<position_type>& anchor, const size_type& size) {
		//	return button5(font, label, position_type{ anchor->x, anchor->y - size.y }, size);
		//}
		//bool button5(const Font& font, const StringView& label, const Arg::bottomCenter_<position_type>& anchor, const size_type& size) {
		//	return button5(font, label, position_type{ anchor->x / 2, anchor->y - size.y }, size);
		//}
		//bool button5(const Font& font, const StringView& label, const Arg::bottomRight_<position_type>& anchor, const size_type& size) {
		//	return button5(font, label, position_type{ anchor->x - size.x, anchor->y - size.y }, size);
		//}
		//bool button5(const Font& font, const StringView& label, const position_type& pos, const size_type& size) {
		//	const RectF button_rectf{ pos, size };
		//	const RoundRect button_roundrect = button_rectf.rounded(Min(size.x / button5_roundrect_rate, size.y / button5_roundrect_rate));
		//	if (button_roundrect.leftPressed()) button_roundrect.draw(button5_color_press);
		//	else if (button_roundrect.mouseOver()) button_roundrect.draw(button5_color_mouseover);
		//	else button_roundrect.draw(button5_color_release);
		//	button_roundrect.drawFrame(Min(size.x, size.y) / 50.0, button5_color_frame);
		//	draw_button_label(label, button_rectf, font, button5_color_label);
		//	return is_click_button(button_roundrect);
		//}



	}
}
