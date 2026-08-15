namespace N38 {
  export const base: number = 38;
  export function add(x: number): number { return base + x; }
}
function over38(x: number): number;
function over38(x: string): string;
function over38(x: number | string): number | string { return x; }
abstract class A38 { abstract f(x: number): number; }
class B38 extends A38 { override f(x: number): number { return x + 1; } }
enum E38 { A = 5 / 2, B, C = 2 ** 3, D }
const angle38 = <number>(38 + 1);
const check38 = {x: angle38} satisfies {x: number};
console.log(N38.add(1), over38(38), new B38().f(1), E38.B, E38.D, check38.x);
