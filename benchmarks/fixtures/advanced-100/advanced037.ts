namespace N37 {
  export const base: number = 37;
  export function add(x: number): number { return base + x; }
}
function over37(x: number): number;
function over37(x: string): string;
function over37(x: number | string): number | string { return x; }
abstract class A37 { abstract f(x: number): number; }
class B37 extends A37 { override f(x: number): number { return x + 1; } }
enum E37 { A = 5 / 2, B, C = 2 ** 3, D }
const angle37 = <number>(37 + 1);
const check37 = {x: angle37} satisfies {x: number};
console.log(N37.add(1), over37(37), new B37().f(1), E37.B, E37.D, check37.x);
