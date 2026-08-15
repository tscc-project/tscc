interface Item332 { id: number; name: string; }
type Key332 = string | number;
export function make332(id: number, name: string): Item332 {
  const value: Item332 = {id, name};
  return value as Item332;
}
export const item332: Item332 = make332(332, "item-332");
