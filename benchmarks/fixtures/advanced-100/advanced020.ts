namespace N20 {
  export const base: number = 20;
  export function add(x: number): number { return base + x; }
}
function over20(x: number): number;
function over20(x: string): string;
function over20(x: number | string): number | string { return x; }
abstract class A20 { abstract f(x: number): number; }
class B20 extends A20 { override f(x: number): number { return x + 1; } }
enum E20 { A = 5 / 2, B, C = 2 ** 3, D }
const angle20 = <number>(20 + 1);
const check20 = {x: angle20} satisfies {x: number};
console.log(N20.add(1), over20(20), new B20().f(1), E20.B, E20.D, check20.x);
