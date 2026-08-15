namespace N11 {
  export const base: number = 11;
  export function add(x: number): number { return base + x; }
}
function over11(x: number): number;
function over11(x: string): string;
function over11(x: number | string): number | string { return x; }
abstract class A11 { abstract f(x: number): number; }
class B11 extends A11 { override f(x: number): number { return x + 1; } }
enum E11 { A = 5 / 2, B, C = 2 ** 3, D }
const angle11 = <number>(11 + 1);
const check11 = {x: angle11} satisfies {x: number};
console.log(N11.add(1), over11(11), new B11().f(1), E11.B, E11.D, check11.x);
