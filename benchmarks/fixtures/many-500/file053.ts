interface Item53 { id: number; name: string; }
type Key53 = string | number;
export function make53(id: number, name: string): Item53 {
  const value: Item53 = {id, name};
  return value as Item53;
}
export const item53: Item53 = make53(53, "item-53");
