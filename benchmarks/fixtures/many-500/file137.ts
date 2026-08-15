interface Item137 { id: number; name: string; }
type Key137 = string | number;
export function make137(id: number, name: string): Item137 {
  const value: Item137 = {id, name};
  return value as Item137;
}
export const item137: Item137 = make137(137, "item-137");
