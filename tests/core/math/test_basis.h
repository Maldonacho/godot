/*************************************************************************/
/*  test_basis.h                                                         */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2022 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2022 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#ifndef TEST_BASIS_H
#define TEST_BASIS_H

#include "core/math/basis.h"
#include "core/math/random_number_generator.h"

#include "tests/test_macros.h"

namespace TestBasis {

enum RotOrder {
	EulerXYZ,
	EulerXZY,
	EulerYZX,
	EulerYXZ,
	EulerZXY,
	EulerZYX
};

Vector3 deg_to_rad(const Vector3 &p_rotation) {
	return p_rotation / 180.0 * Math_PI;
}

Vector3 rad2deg(const Vector3 &p_rotation) {
	return p_rotation / Math_PI * 180.0;
}

Basis EulerToBasis(RotOrder mode, const Vector3 &p_rotation) {
	Basis ret;
	switch (mode) {
		case EulerXYZ:
			ret.set_euler(p_rotation, Basis::EULER_ORDER_XYZ);
			break;

		case EulerXZY:
			ret.set_euler(p_rotation, Basis::EULER_ORDER_XZY);
			break;

		case EulerYZX:
			ret.set_euler(p_rotation, Basis::EULER_ORDER_YZX);
			break;

		case EulerYXZ:
			ret.set_euler(p_rotation, Basis::EULER_ORDER_YXZ);
			break;

		case EulerZXY:
			ret.set_euler(p_rotation, Basis::EULER_ORDER_ZXY);
			break;

		case EulerZYX:
			ret.set_euler(p_rotation, Basis::EULER_ORDER_ZYX);
			break;

		default:
			// If you land here, Please integrate all rotation orders.
			FAIL("This is not unreachable.");
	}

	return ret;
}

Vector3 BasisToEuler(RotOrder mode, const Basis &p_rotation) {
	switch (mode) {
		case EulerXYZ:
			return p_rotation.get_euler(Basis::EULER_ORDER_XYZ);

		case EulerXZY:
			return p_rotation.get_euler(Basis::EULER_ORDER_XZY);

		case EulerYZX:
			return p_rotation.get_euler(Basis::EULER_ORDER_YZX);

		case EulerYXZ:
			return p_rotation.get_euler(Basis::EULER_ORDER_YXZ);

		case EulerZXY:
			return p_rotation.get_euler(Basis::EULER_ORDER_ZXY);

		case EulerZYX:
			return p_rotation.get_euler(Basis::EULER_ORDER_ZYX);

		default:
			// If you land here, Please integrate all rotation orders.
			FAIL("This is not unreachable.");
			return Vector3();
	}
}

String get_rot_order_name(RotOrder ro) {
	switch (ro) {
		case EulerXYZ:
			return "XYZ";
		case EulerXZY:
			return "XZY";
		case EulerYZX:
			return "YZX";
		case EulerYXZ:
			return "YXZ";
		case EulerZXY:
			return "ZXY";
		case EulerZYX:
			return "ZYX";
		default:
			return "[Not supported]";
	}
}

void test_rotation(Vector3 deg_original_euler, RotOrder rot_order) {
	// This test:
	// 1. Converts the rotation vector from deg to rad.
	// 2. Converts euler to basis.
	// 3. Converts the above basis back into euler.
	// 4. Converts the above euler into basis again.
	// 5. Compares the basis obtained in step 2 with the basis of step 4
	//
	// The conversion "basis to euler", done in the step 3, may be different from
	// the original euler, even if the final rotation are the same.
	// This happens because there are more ways to represents the same rotation,
	// both valid, using eulers.
	// For this reason is necessary to convert that euler back to basis and finally
	// compares it.
	//
	// In this way we can assert that both functions: basis to euler / euler to basis
	// are correct.

	// Euler to rotation
	const Vector3 original_euler = deg_to_rad(deg_original_euler);
	const Basis to_rotation = EulerToBasis(rot_order, original_euler);

	// Euler from rotation
	const Vector3 euler_from_rotation = BasisToEuler(rot_order, to_rotation);
	const Basis rotation_from_computed_euler = EulerToBasis(rot_order, euler_from_rotation);

	Basis res = to_rotation.inverse() * rotation_from_computed_euler;

	CHECK_MESSAGE((res.get_column(0) - Vector3(1.0, 0.0, 0.0)).length() <= 0.1, vformat("Fail due to X %s\n", String(res.get_column(0))).utf8().ptr());
	CHECK_MESSAGE((res.get_column(1) - Vector3(0.0, 1.0, 0.0)).length() <= 0.1, vformat("Fail due to Y %s\n", String(res.get_column(1))).utf8().ptr());
	CHECK_MESSAGE((res.get_column(2) - Vector3(0.0, 0.0, 1.0)).length() <= 0.1, vformat("Fail due to Z %s\n", String(res.get_column(2))).utf8().ptr());

	// Double check `to_rotation` decomposing with XYZ rotation order.
	const Vector3 euler_xyz_from_rotation = to_rotation.get_euler(Basis::EULER_ORDER_XYZ);
	Basis rotation_from_xyz_computed_euler = Basis::from_euler(euler_xyz_from_rotation, Basis::EULER_ORDER_XYZ);

	res = to_rotation.inverse() * rotation_from_xyz_computed_euler;

	CHECK_MESSAGE((res.get_column(0) - Vector3(1.0, 0.0, 0.0)).length() <= 0.1, vformat("Double check with XYZ rot order failed, due to X %s\n", String(res.get_column(0))).utf8().ptr());
	CHECK_MESSAGE((res.get_column(1) - Vector3(0.0, 1.0, 0.0)).length() <= 0.1, vformat("Double check with XYZ rot order failed, due to Y %s\n", String(res.get_column(1))).utf8().ptr());
	CHECK_MESSAGE((res.get_column(2) - Vector3(0.0, 0.0, 1.0)).length() <= 0.1, vformat("Double check with XYZ rot order failed, due to Z %s\n", String(res.get_column(2))).utf8().ptr());

	INFO(vformat("Rotation order: %s\n.", get_rot_order_name(rot_order)).utf8().ptr());
	INFO(vformat("Original Rotation: %s\n", String(deg_original_euler)).utf8().ptr());
	INFO(vformat("Quaternion to rotation order: %s\n", String(rad2deg(euler_from_rotation))).utf8().ptr());
}

TEST_CASE("[Basis] Euler conversions") {
	Vector<RotOrder> rotorder_to_test;
	rotorder_to_test.push_back(EulerXYZ);
	rotorder_to_test.push_back(EulerXZY);
	rotorder_to_test.push_back(EulerYZX);
	rotorder_to_test.push_back(EulerYXZ);
	rotorder_to_test.push_back(EulerZXY);
	rotorder_to_test.push_back(EulerZYX);

	Vector<Vector3> vectors_to_test;

	// Test the special cases.
	vectors_to_test.push_back(Vector3(0.0, 0.0, 0.0));
	vectors_to_test.push_back(Vector3(0.5, 0.5, 0.5));
	vectors_to_test.push_back(Vector3(-0.5, -0.5, -0.5));
	vectors_to_test.push_back(Vector3(40.0, 40.0, 40.0));
	vectors_to_test.push_back(Vector3(-40.0, -40.0, -40.0));
	vectors_to_test.push_back(Vector3(0.0, 0.0, -90.0));
	vectors_to_test.push_back(Vector3(0.0, -90.0, 0.0));
	vectors_to_test.push_back(Vector3(-90.0, 0.0, 0.0));
	vectors_to_test.push_back(Vector3(0.0, 0.0, 90.0));
	vectors_to_test.push_back(Vector3(0.0, 90.0, 0.0));
	vectors_to_test.push_back(Vector3(90.0, 0.0, 0.0));
	vectors_to_test.push_back(Vector3(0.0, 0.0, -30.0));
	vectors_to_test.push_back(Vector3(0.0, -30.0, 0.0));
	vectors_to_test.push_back(Vector3(-30.0, 0.0, 0.0));
	vectors_to_test.push_back(Vector3(0.0, 0.0, 30.0));
	vectors_to_test.push_back(Vector3(0.0, 30.0, 0.0));
	vectors_to_test.push_back(Vector3(30.0, 0.0, 0.0));
	vectors_to_test.push_back(Vector3(0.5, 50.0, 20.0));
	vectors_to_test.push_back(Vector3(-0.5, -50.0, -20.0));
	vectors_to_test.push_back(Vector3(0.5, 0.0, 90.0));
	vectors_to_test.push_back(Vector3(0.5, 0.0, -90.0));
	vectors_to_test.push_back(Vector3(360.0, 360.0, 360.0));
	vectors_to_test.push_back(Vector3(-360.0, -360.0, -360.0));
	vectors_to_test.push_back(Vector3(-90.0, 60.0, -90.0));
	vectors_to_test.push_back(Vector3(90.0, 60.0, -90.0));
	vectors_to_test.push_back(Vector3(90.0, -60.0, -90.0));
	vectors_to_test.push_back(Vector3(-90.0, -60.0, -90.0));
	vectors_to_test.push_back(Vector3(-90.0, 60.0, 90.0));
	vectors_to_test.push_back(Vector3(90.0, 60.0, 90.0));
	vectors_to_test.push_back(Vector3(90.0, -60.0, 90.0));
	vectors_to_test.push_back(Vector3(-90.0, -60.0, 90.0));
	vectors_to_test.push_back(Vector3(60.0, 90.0, -40.0));
	vectors_to_test.push_back(Vector3(60.0, -90.0, -40.0));
	vectors_to_test.push_back(Vector3(-60.0, -90.0, -40.0));
	vectors_to_test.push_back(Vector3(-60.0, 90.0, 40.0));
	vectors_to_test.push_back(Vector3(60.0, 90.0, 40.0));
	vectors_to_test.push_back(Vector3(60.0, -90.0, 40.0));
	vectors_to_test.push_back(Vector3(-60.0, -90.0, 40.0));
	vectors_to_test.push_back(Vector3(-90.0, 90.0, -90.0));
	vectors_to_test.push_back(Vector3(90.0, 90.0, -90.0));
	vectors_to_test.push_back(Vector3(90.0, -90.0, -90.0));
	vectors_to_test.push_back(Vector3(-90.0, -90.0, -90.0));
	vectors_to_test.push_back(Vector3(-90.0, 90.0, 90.0));
	vectors_to_test.push_back(Vector3(90.0, 90.0, 90.0));
	vectors_to_test.push_back(Vector3(90.0, -90.0, 90.0));
	vectors_to_test.push_back(Vector3(20.0, 150.0, 30.0));
	vectors_to_test.push_back(Vector3(20.0, -150.0, 30.0));
	vectors_to_test.push_back(Vector3(-120.0, -150.0, 30.0));
	vectors_to_test.push_back(Vector3(-120.0, -150.0, -130.0));
	vectors_to_test.push_back(Vector3(120.0, -150.0, -130.0));
	vectors_to_test.push_back(Vector3(120.0, 150.0, -130.0));
	vectors_to_test.push_back(Vector3(120.0, 150.0, 130.0));

	for (int h = 0; h < rotorder_to_test.size(); h += 1) {
		for (int i = 0; i < vectors_to_test.size(); i += 1) {
			test_rotation(vectors_to_test[i], rotorder_to_test[h]);
		}
	}
}

TEST_CASE("[Stress][Basis] Euler conversions") {
	Vector<RotOrder> rotorder_to_test;
	rotorder_to_test.push_back(EulerXYZ);
	rotorder_to_test.push_back(EulerXZY);
	rotorder_to_test.push_back(EulerYZX);
	rotorder_to_test.push_back(EulerYXZ);
	rotorder_to_test.push_back(EulerZXY);
	rotorder_to_test.push_back(EulerZYX);

	Vector<Vector3> vectors_to_test;
	// Add 1000 random vectors with weirds numbers.
	RandomNumberGenerator rng;
	for (int _ = 0; _ < 1000; _ += 1) {
		vectors_to_test.push_back(Vector3(
				rng.randf_range(-1800, 1800),
				rng.randf_range(-1800, 1800),
				rng.randf_range(-1800, 1800)));
	}

	for (int h = 0; h < rotorder_to_test.size(); h += 1) {
		for (int i = 0; i < vectors_to_test.size(); i += 1) {
			test_rotation(vectors_to_test[i], rotorder_to_test[h]);
		}
	}
}

TEST_CASE("[Basis] Set axis angle") {
	Vector3 axis;
	real_t angle;
	real_t pi = (real_t)Math_PI;

	// Testing the singularity when the angle is 0°.
	Basis identity(1, 0, 0, 0, 1, 0, 0, 0, 1);
	identity.get_axis_angle(axis, angle);
	CHECK(angle == 0);

	// Testing the singularity when the angle is 180°.
	Basis singularityPi(-1, 0, 0, 0, 1, 0, 0, 0, -1);
	singularityPi.get_axis_angle(axis, angle);
	CHECK(Math::is_equal_approx(angle, pi));

	// Testing reversing the an axis (of an 30° angle).
	float cos30deg = Math::cos(Math::deg_to_rad((real_t)30.0));
	Basis z_positive(cos30deg, -0.5, 0, 0.5, cos30deg, 0, 0, 0, 1);
	Basis z_negative(cos30deg, 0.5, 0, -0.5, cos30deg, 0, 0, 0, 1);

	z_positive.get_axis_angle(axis, angle);
	CHECK(Math::is_equal_approx(angle, Math::deg_to_rad((real_t)30.0)));
	CHECK(axis == Vector3(0, 0, 1));

	z_negative.get_axis_angle(axis, angle);
	CHECK(Math::is_equal_approx(angle, Math::deg_to_rad((real_t)30.0)));
	CHECK(axis == Vector3(0, 0, -1));

	// Testing a rotation of 90° on x-y-z.
	Basis x90deg(1, 0, 0, 0, 0, -1, 0, 1, 0);
	x90deg.get_axis_angle(axis, angle);
	CHECK(Math::is_equal_approx(angle, pi / (real_t)2));
	CHECK(axis == Vector3(1, 0, 0));

	Basis y90deg(0, 0, 1, 0, 1, 0, -1, 0, 0);
	y90deg.get_axis_angle(axis, angle);
	CHECK(axis == Vector3(0, 1, 0));

	Basis z90deg(0, -1, 0, 1, 0, 0, 0, 0, 1);
	z90deg.get_axis_angle(axis, angle);
	CHECK(axis == Vector3(0, 0, 1));

	// Regression test: checks that the method returns a small angle (not 0).
	Basis tiny(1, 0, 0, 0, 0.9999995, -0.001, 0, 001, 0.9999995); // The min angle possible with float is 0.001rad.
	tiny.get_axis_angle(axis, angle);
	CHECK(Math::is_equal_approx(angle, (real_t)0.001, (real_t)0.0001));

	// Regression test: checks that the method returns an angle which is a number (not NaN)
	Basis bugNan(1.00000024, 0, 0.000100001693, 0, 1, 0, -0.000100009143, 0, 1.00000024);
	bugNan.get_axis_angle(axis, angle);
	CHECK(!Math::is_nan(angle));
}

TEST_CASE("[Basis] Finite number checks") {
	const Vector3 x(0, 1, 2);
	const Vector3 infinite(NAN, NAN, NAN);

	CHECK_MESSAGE(
			Basis(x, x, x).is_finite(),
			"Basis with all components finite should be finite");

	CHECK_FALSE_MESSAGE(
			Basis(infinite, x, x).is_finite(),
			"Basis with one component infinite should not be finite.");
	CHECK_FALSE_MESSAGE(
			Basis(x, infinite, x).is_finite(),
			"Basis with one component infinite should not be finite.");
	CHECK_FALSE_MESSAGE(
			Basis(x, x, infinite).is_finite(),
			"Basis with one component infinite should not be finite.");

	CHECK_FALSE_MESSAGE(
			Basis(infinite, infinite, x).is_finite(),
			"Basis with two components infinite should not be finite.");
	CHECK_FALSE_MESSAGE(
			Basis(infinite, x, infinite).is_finite(),
			"Basis with two components infinite should not be finite.");
	CHECK_FALSE_MESSAGE(
			Basis(x, infinite, infinite).is_finite(),
			"Basis with two components infinite should not be finite.");

	CHECK_FALSE_MESSAGE(
			Basis(infinite, infinite, infinite).is_finite(),
			"Basis with three components infinite should not be finite.");
}

} // namespace TestBasis

#endif // TEST_BASIS_H
