namespace N4 {
  export const base: number = 4;
  export function add(x: number): number { return base + x; }
}
function over4(x: number): number;
function over4(x: string): string;
function over4(x: number | string): number | string { return x; }
abstract class A4 { abstract f(x: number): number; }
class B4 extends A4 { override f(x: number): number { return x + 1; } }
enum E4 { A = 5 / 2, B, C = 2 ** 3, D }
const angle4 = <number>(4 + 1);
const check4 = {x: angle4} satisfies {x: number};
console.log(N4.add(1), over4(4), new B4().f(1), E4.B, E4.D, check4.x);
