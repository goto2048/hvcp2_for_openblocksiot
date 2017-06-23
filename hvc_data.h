/* HVC P2 data output */

struct hvc_header {
	int8_t body;	/* number of human bodies */
	int8_t hand;	/* number of human hands */
	int8_t face;	/* humber of human faces */
	int8_t res;	/* reserved ( no use ) */
} hvc_header;

struct hvc_body { 
	int16_t	x;	/* x coordinate */
	int16_t	y;	/* y coordinate */
	int16_t	size;	/* size */
	int16_t	rel;	/* reliability */
} hvc_body[35];

struct hvc_hand { 
	int16_t	x;	/* x coordinate */
	int16_t	y;	/* y coordinate */
	int16_t	size;	/* size */
	int16_t	rel;	/* reliability */
} hvc_hand[35];

struct hvc_face { 
	int16_t	x;	/* x coordinate */
	int16_t	y;	/* y coordinate */
	int16_t	size;	/* size */
	int16_t	rel_face;	/* reliability */
	int16_t	h_angle;	/* horizontal direction angle */
	int16_t	v_angle;	/* virtical direction angle */
	int16_t	f_angle;	/* face inclination */
	int16_t	rel_angle;	/* reliability */
	int8_t	age;
	int16_t	rel_age;
	int8_t	sex;
	int16_t	rel_sex;
	int8_t	sight;
	int16_t	rel_sight;
	int16_t	eye_left;
	int16_t	eye_right;
	int8_t	exp_less;
	int8_t	exp_joy;
	int8_t	exp_surprise;
	int8_t	exp_anger;
	int8_t	exp_sad;
	int8_t	exp_positive;
	uint16_t	userid;
	uint16_t	score;
} hvc_face[35];
