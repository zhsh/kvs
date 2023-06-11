eps = 0.01;
w = 0.8;

nano_length = 44 + 0.5;
nano_width = 17.7 + 0.5;
nano_height = 1.7 + 0.5;

nano_anthena_length = 15.3 + 0.5;
nano_anthena_width = 10 + 0.5;
nano_anthena_height = 4 + 0.5;

nano_usb_length = 5.6 + 0.5;
nano_usb_width = 7.6 + 0.5;
nano_usb_height = 4.3 + 0.5;

nano_middle_width = 13.7;


battery_holder_length = 23.7 + 0.5;
battery_holder_width = 28 + 0.5;
battery_holder_height = 1.6 + 0.5;
battery_d = 20 + 0.5;
battery_and_holder_length = 25.15;
battery_and_holder_height = 5.5 + 0.5;
battery_upper_length = 15 + 0.5;
battery_upper_width = 21.3 + 0.5;

battery_hole_d = 2.5;
battery_hole_y_offset = (20.7 + battery_hole_d) / 2;
battery_hole_x_offset = (16 + battery_hole_d) / 2;

hole_from_edge_x = battery_holder_length / 2 - battery_hole_x_offset;
hole_from_edge_y = battery_holder_width / 2 - battery_hole_y_offset;


module nano() {
  translate([0,0, nano_height/2])
  cube([nano_length,nano_width,nano_height], center = true);

//  translate([5,0, nano_usb_height/2])
//  cube([nano_length - nano_usb_length - nano_anthena_length - 2,
//       nano_middle_width, nano_usb_height], center = true);

  translate([-nano_length/2 + nano_anthena_length / 2,
      0,
      nano_anthena_height / 2])
    cube([nano_anthena_length,
        nano_anthena_width,
        nano_anthena_height], center = true);

  translate([nano_length/2 - nano_usb_length / 2,
      0,
      nano_usb_height / 2])
    cube([nano_usb_length,
        nano_usb_width,
        nano_usb_height], center = true);

  translate([-nano_length/2 + 6.5, - 1.4 + nano_width/2, 0]) cylinder(d=2, h=6, $fn=10);
  translate([ nano_length/2 - 6.0, - 1.4 + nano_width/2, 0]) cylinder(d=2, h=6, $fn=10);
}

module battery() {
  difference() {
    union() {
      translate([0,0,battery_holder_height/2]) cube([battery_holder_length,
          battery_holder_width,
          battery_holder_height
      ], center = true);
      target_offset = battery_and_holder_length - battery_holder_length;
      edge_offset = battery_holder_length / 2 - battery_d / 2;
      translate([-edge_offset - target_offset, 0, battery_holder_height]) {
        cylinder(d=battery_d, h=battery_and_holder_height - battery_holder_height - 0.2, $fn=40);
        translate([0,0, (battery_and_holder_height-battery_holder_height)/2])
          cube([battery_upper_length,
              battery_upper_width,
              battery_and_holder_height - battery_holder_height], center = true);
      }
    };
    union() {
      translate([-battery_hole_x_offset, -battery_hole_y_offset, -eps])
        cylinder(d = battery_hole_d, h=battery_and_holder_height + eps * 2, $fn=10);
      translate([ battery_hole_x_offset, -battery_hole_y_offset, -eps])
        cylinder(d = battery_hole_d, h=battery_and_holder_height + eps * 2, $fn=10);
      translate([-battery_hole_x_offset,  battery_hole_y_offset, -eps])
        cylinder(d = battery_hole_d, h=battery_and_holder_height + eps * 2, $fn=10);
      translate([ battery_hole_x_offset,  battery_hole_y_offset, -eps])
        cylinder(d = battery_hole_d, h=battery_and_holder_height + eps * 2, $fn=10);
    };
  }
}

button_width = 6.18 + 0.5;
button_height = 3.5 + 0.5;
button_d = 3.4 + 0.5;
button_d_h = 4.9 + 0.5;

module button() {
  translate([0,0,button_height/2])
  cube([button_width, button_width, button_height], center = true);
  cylinder(d = button_d, h = button_d_h, $fn = 20);
}

pad = 0.2;

base_length = w * 2 + nano_length;
base_width = w * 2 + battery_and_holder_length;
base_wall_height = w + nano_usb_height + pad + battery_and_holder_height + w;

module outter_box() {
  translate([-w,-w, 0])
    cube([base_length, base_width, base_wall_height]);
}

module inner_box() {
  translate([0,0,w])
    cube([base_length - 2 * w, base_width - 2 * w, base_wall_height - 2 * w]);
}

screw1_height = battery_and_holder_height - battery_holder_height + w;

screw2_height = battery_and_holder_height + w;
screw_outer_d = 10;

module screw1_base_sub(height) {
  cup_height = height - 2;
  translate([hole_from_edge_y, hole_from_edge_x, -eps])
    cylinder(d=7, h = cup_height, $fn=100);

  translate([-w - eps,-w -eps, -eps])
  cube([hole_from_edge_y + w, hole_from_edge_x + w, cup_height]);

  translate([hole_from_edge_y, hole_from_edge_x, -eps])
    cylinder(d=3.5, h = height + 2 * eps, $fn=100);
}

module screw1_base_add(height) {
  intersection() {
    translate([hole_from_edge_y, hole_from_edge_x, 0])
  color("red")
      cylinder(d=screw_outer_d, h = height, $fn=100);
    outter_box();
  }
}

module base() {
  translate([-w,-w, 0]) {
    cube([base_length, base_width, w]);

    cube([base_length, w, base_wall_height - w]);

    translate([0,base_width - w,0])
      cube([base_length, w, base_wall_height]);

    cube([w, base_width, base_wall_height]);
    translate([base_length -w, 0, 0])
      cube([w, base_width, base_wall_height]);
  }
}

button_offset_x = base_length - 2 * w - button_width / 2 - screw_outer_d / 2 - hole_from_edge_y;
button_offset_y = button_width / 2;

nano_offset_y = base_width - nano_width - 2 * w;
nano_offset_z = battery_and_holder_height + pad;

module components() {
  color("green")
    translate([button_offset_x, button_offset_y, w])
      button();

  color("blue")
    translate([battery_holder_width / 2,
        battery_holder_length / 2,
        battery_and_holder_height + w])
    rotate(-90, [0,0,1])
    rotate(180, [1,0,0])
    battery();

  color("cyan")
    translate([nano_length/2, nano_width/2 + nano_offset_y, w + nano_offset_z])
    rotate(180, [0,0,1])
    nano();
}

module corner() {
  translate([-w, base_width - 1 - 2 * w, base_wall_height])
    rotate(90, [0,1,0])
    linear_extrude(base_length)
    polygon([[0,0],[0,1],[1,1]]);
}

module bottom() {
  difference() {
    union() {
      base();
      screw1_base_add(screw1_height);
      translate([base_length - 2 * w, 0, 0])
        mirror([1,0,0])
        screw1_base_add(screw1_height);
    };
    union() {
      screw1_base_sub(screw1_height);
      translate([base_length - 2 * w, 0, 0])
        mirror([1,0,0])
        screw1_base_sub(screw1_height);
    }
  }
  translate([button_offset_x - button_width/2 - w, 0, w])
    cube([w, button_width, button_height]);

  translate([button_offset_x - w/2, button_width, w])
    cube([w, 3, button_height]);


  corner();
  translate([0,0,-1])
    corner();
}

screw_thread = 2.9;

module top_mount(extra) {
  difference() {
    z = nano_offset_z + w - extra;
    height = base_wall_height - z;
    translate([hole_from_edge_y, hole_from_edge_x, z])
      cylinder(d=6.0, h = height, $fn=100);

    translate([hole_from_edge_y, hole_from_edge_x, z - eps])
      cylinder(d=screw_thread, h = height + 2 * eps, $fn=100);
  }
}

// Button channel
button_stick_out = 1;
channel_outer_d = 11;
channel_inner_d = 11 - 0.6 * 2;
channel_offset_z = nano_offset_z + nano_height + w;
channel_height = base_wall_height - channel_offset_z;
channel_top_z = 0.8;
channel_inner_top_d = channel_inner_d - 0.8;

button_cap_top_d = channel_inner_top_d - 0.2;
button_cap_d = channel_inner_d - 0.2;
button_cap_height = 1.6 + button_stick_out;
button_axe_d = 6;

module button_channel_inner() {
  translate([button_offset_x, button_offset_y, channel_offset_z - eps]) {
    cylinder(d=channel_inner_d, h=channel_height - channel_top_z + 2 * eps, $fn=100);
    translate([0,0, channel_height - channel_top_z - eps])
      cylinder(d=channel_inner_top_d, h = channel_top_z + 3 * eps, $fn=100);
  }
};

module button_cap0() {
  translate([button_offset_x, button_offset_y, channel_offset_z]) {
    translate([0,0, channel_height - button_cap_height])
      cylinder(d=button_cap_d, h=button_cap_height - channel_top_z - button_stick_out, $fn=100);

    translate([0,0, channel_height - channel_top_z - button_stick_out])
      cylinder(d=button_cap_top_d, h = channel_top_z + button_stick_out, $fn=100);

  }
  translate([button_offset_x, button_offset_y, button_d_h + w - button_stick_out]) {
    cylinder(d=button_axe_d, h=button_stick_out + base_wall_height - button_d_h - w, $fn=100);
  }
}

module button_cap() {
  intersection() {
    difference() {
      button_cap0();
      bottom();
    };
    outter_box();
  }
}

module top() {
  intersection() {
    union() {
      top_mount(0);
      translate([base_length - 2 * w, 0, 0]) mirror([1,0,0]) top_mount(battery_holder_height);
    };
    inner_box();
  }

  intersection() {
    difference() {
      union() {
        color("red")
          translate([0,-w,base_wall_height-w])
          cube([base_length - 2 * w, base_width - 2 * w + 1, w]);
        translate([button_offset_x, button_offset_y, channel_offset_z])
          cylinder(d=channel_outer_d, h=channel_height, $fn=100);
      };
      union() {
        button_channel_inner();
        bottom();
        d = 1.5;
        translate([d/2 + 0.8, nano_offset_y + 2.5, 0]) cylinder(d=d, h=20, $fn=30);
        translate([d/2 + 0.8, base_width - 2.5 - 2 * w, 0]) cylinder(d=d, h=20, $fn=30);
      }
    }
    outter_box();
  };
}

// bottom();
// components();
// button_cap();
top();
