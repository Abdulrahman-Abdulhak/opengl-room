in vec3 vColor;
in vec3 vWorldPos;

out vec4 FragColor;

uniform sampler2D uTex;
uniform float uTile;       // e.g. 1.0, 2.0, 5.0
uniform vec3 uRoomCenter;  // usually model translation (or 0 if centered)
uniform vec3 uHalfSize;    // (width/2, height/2, depth/2) in WORLD space

void main() {
    // Position relative to room center
    vec3 p = vWorldPos - uRoomCenter;

    // Determine which face this fragment belongs to by closeness to the room planes
    float dx = abs(abs(p.x) - uHalfSize.x);
    float dy = abs(abs(p.y) - uHalfSize.y);
    float dz = abs(abs(p.z) - uHalfSize.z);

    vec2 uv;

    // Project UVs based on dominant axis
    if (dx < dy && dx < dz) {
        // left/right wall: use Z and Y
        uv = vec2(p.z, p.y);
    } else if (dz < dx && dz < dy) {
        // front/back wall: use X and Y
        uv = vec2(p.x, p.y);
    } else {
        // floor/ceiling: use X and Z
        uv = vec2(p.x, p.z);
    }

    uv *= uTile; // tiling happens here

    vec4 tex = texture(uTex, uv);
    FragColor = tex * vec4(vColor, 1.0);
}
