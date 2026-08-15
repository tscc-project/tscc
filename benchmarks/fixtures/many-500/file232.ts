interface Item232 { id: number; name: string; }
type Key232 = string | number;
export function make232(id: number, name: string): Item232 {
  const value: Item232 = {id, name};
  return value as Item232;
}
export const item232: Item232 = make232(232, "item-232");
