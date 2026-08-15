interface Item360 { id: number; name: string; }
type Key360 = string | number;
export function make360(id: number, name: string): Item360 {
  const value: Item360 = {id, name};
  return value as Item360;
}
export const item360: Item360 = make360(360, "item-360");
