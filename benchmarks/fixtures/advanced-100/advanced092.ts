namespace N92 {
  export const base: number = 92;
  export function add(x: number): number { return base + x; }
}
function over92(x: number): number;
function over92(x: string): string;
function over92(x: number | string): number | string { return x; }
abstract class A92 { abstract f(x: number): number; }
class B92 extends A92 { override f(x: number): number { return x + 1; } }
enum E92 { A = 5 / 2, B, C = 2 ** 3, D }
const angle92 = <number>(92 + 1);
const check92 = {x: angle92} satisfies {x: number};
console.log(N92.add(1), over92(92), new B92().f(1), E92.B, E92.D, check92.x);
