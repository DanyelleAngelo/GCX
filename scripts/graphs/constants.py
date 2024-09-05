LINE_STYLE = ["--", "-.", "-","--"]
MARKER = ["*","D", "H", "x","s", ">", "v", "^", "o"]

COLOR_MAP= {
    'compression_time': {
        "default_color": "#007599", "highlighted_color": "#00ffff", "others": "#ffa600"
    },
    'decompression_time': {
        "default_color": "#007599", "highlighted_color": "#00ffff", "others": "#ffa600"
    },
    'compressed_size': {
        "default_color": "#bc5090", "highlighted_color": "#ffa600", "others": "#0056ff"
    },
    'peak_comp': {
         "default_color": "#ef6649", "highlighted_color": "#ef9e2b"
    },
    'peak_decomp': {
         "default_color": "#ef6649", "highlighted_color": "#ef9e2b"
    },
    'line': ["#3f38ff", "#ffa600",  "green", "#ff4444", "#8900ff","#a90000"],
    'compressed_size_bar': ["#665191", "#a05195", "#d45087", "#f95d6a", "#f95d6a"],
    'compression_time_bar': ["#004c6d", "#007599", "#00cfe3", "#00ffff","#ffa600" ],
    'decompression_time_bar': ["#004c6d", "#007599", "#00cfe3" ,"#00ffff","#ffa600" ],
    'peak_comp_bar': [ "#12436D",  "#28A197", "#801650", "#ef9e2b", "#ef6649"]
}

COLOR_MAP['peak_decomp_bar'] = COLOR_MAP['peak_comp_bar']