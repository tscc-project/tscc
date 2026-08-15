namespace N54 {
  export const base: number = 54;
  export function add(x: number): number { return base + x; }
}
function over54(x: number): number;
function over54(x: string): string;
function over54(x: number | string): number | string { return x; }
abstract class A54 { abstract f(x: number): number; }
class B54 extends A54 { override f(x: number): number { return x + 1; } }
enum E54 { A = 5 / 2, B, C = 2 ** 3, D }
const angle54 = <number>(54 + 1);
const check54 = {x: angle54} satisfies {x: number};
console.log(N54.add(1), over54(54), new B54().f(1), E54.B, E54.D, check54.x);
