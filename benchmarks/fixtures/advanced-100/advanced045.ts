namespace N45 {
  export const base: number = 45;
  export function add(x: number): number { return base + x; }
}
function over45(x: number): number;
function over45(x: string): string;
function over45(x: number | string): number | string { return x; }
abstract class A45 { abstract f(x: number): number; }
class B45 extends A45 { override f(x: number): number { return x + 1; } }
enum E45 { A = 5 / 2, B, C = 2 ** 3, D }
const angle45 = <number>(45 + 1);
const check45 = {x: angle45} satisfies {x: number};
console.log(N45.add(1), over45(45), new B45().f(1), E45.B, E45.D, check45.x);
