interface Item236 { id: number; name: string; }
type Key236 = string | number;
export function make236(id: number, name: string): Item236 {
  const value: Item236 = {id, name};
  return value as Item236;
}
export const item236: Item236 = make236(236, "item-236");
