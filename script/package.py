from log import *
import common

import os
import sys
import chevron
from pathlib import Path
from shutil import copyfile

demos = [
    ["fire", "OpenGL varjostin, joka jäljittelee palavaa liekkiä"],
    [
        "signaali",
        "Alkuperäinen toteutus piirsi PulseAudion kauttaa siepatun äänisignaalin, mutta selaindemoon on se korvattu siniaalloilla. Nopeankin ääniaallon piirtäminen on tehokasta, koska pisteet piirretään kerran framebuffer -tekstuuriin, jota siirretään tasaisesti eteenpäin suorituksen edetessä. u/i säätävät aallon etenemisnopeutta. j/k säätävät pisteen kokoa. l vaihtaa viiva- ja piste-esityksen välillä.",
    ],
    [
        "disintegrate",
        "OpenGL varjostin, joka edetessään haihduttaa alkuperäisen kuvan tyhjyyteen",
    ],
    ["cubes", "Tippuva kuutio, joka on simuloitu Bullet -fysiikkamoottorilla"],
    ["roiske", "Maalaa sateenkaarilipulla framebuffer -tekstuuriin."],
    ["sprite", "Yksinkertainen animoitu sateenkaarilippu"],
    [
        "text",
        "Piirtää tekstiä ja tukee esimerkiksi useampaa riviä, reunaan ja keskelle sovittamista. Lataa fontin FreeType2 -kirjastolla ja luo kirjaimista taulukkotekstuurin.",
    ],
]

dir_name = "pages"


def main():
    skeleton_page_filename = sys.argv[1] if len(sys.argv) > 1 else None
    if skeleton_page_filename is None:
        error(f"Missing skeleton filename")
        return False
    if not os.path.isfile(skeleton_page_filename):
        error(f"Not a (file: {skeleton_page_filename}")
        return False

    demo_page_filename = sys.argv[2] if len(sys.argv) > 2 else None
    if demo_page_filename is None:
        error(f"Missing demo page filename")
        return False
    if not os.path.isfile(demo_page_filename):
        error(f"Not a (file: {demo_page_filename}")
        return False

    skeleton = common.read_file_to_string(skeleton_page_filename)
    common.ensure_dir(dir_name)
    for demo_info in demos:
        demo = demo_info[0]
        desc = demo_info[1]
        important(f"Packaging {demo}")
        common.ensure_dir(os.path.join(dir_name, demo))
        with open(os.path.join(dir_name, demo, "demo.html"), "w") as file:
            file.write(
                chevron.render(
                    skeleton, {"description": desc, "js_filename": f"{demo}.js"}
                )
            )
        log("Wrote html")
        copyfile(f"{demo}.js", os.path.join(dir_name, demo, f"{demo}.js"))
        copyfile(f"{demo}.wasm", os.path.join(dir_name, demo, f"{demo}.wasm"))
        log("Copied scripts")

    demo_page = common.read_file_to_string(demo_page_filename)
    with open(os.path.join(dir_name, "opengl.html"), "w") as file:
        file.write(
            chevron.render(
                demo_page,
                {
                    "description": "Kokoelma erilaisia OpenGL demoja. Tehty noin 6v sitten ja portattu toimimaan modernissa selaimessa.",
                    "links": "".join(
                        [
                            f'<div onClick="window.open(\'./{demo_info[0]}/demo.html\', \'_self\');" class="card columns clickable"><img src="opengl-demo-{demo_info[0]}.png" class="card-image" /><div><p class="card-title">{demo_info[0]}</p><p class="card-content">{demo_info[1]}</p></div></div>'
                            for demo_info in demos
                        ]
                    ),
                },
            )
        )


if __name__ == "__main__":
    main()
