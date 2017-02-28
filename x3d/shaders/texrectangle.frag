uniform sampler2D texRect;

void main(void ) {
  gl_FragColor = texture2D( texRect, vec2( 128. * gl_TexCoord[0].s, 128. * gl_TexCoord[0].t ) );
}
