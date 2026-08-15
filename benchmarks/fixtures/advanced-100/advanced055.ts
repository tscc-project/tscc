namespace N55 {
  export const base: number = 55;
  export function add(x: number): number { return base + x; }
}
function over55(x: number): number;
function over55(x: string): string;
function over55(x: number | string): number | string { return x; }
abstract class A55 { abstract f(x: number): number; }
class B55 extends A55 { override f(x: number): number { return x + 1; } }
enum E55 { A = 5 / 2, B, C = 2 ** 3, D }
const angle55 = <number>(55 + 1);
const check55 = {x: angle55} satisfies {x: number};
console.log(N55.add(1), over55(55), new B55().f(1), E55.B, E55.D, check55.x);
