namespace N25 {
  export const base: number = 25;
  export function add(x: number): number { return base + x; }
}
function over25(x: number): number;
function over25(x: string): string;
function over25(x: number | string): number | string { return x; }
abstract class A25 { abstract f(x: number): number; }
class B25 extends A25 { override f(x: number): number { return x + 1; } }
enum E25 { A = 5 / 2, B, C = 2 ** 3, D }
const angle25 = <number>(25 + 1);
const check25 = {x: angle25} satisfies {x: number};
console.log(N25.add(1), over25(25), new B25().f(1), E25.B, E25.D, check25.x);
