interface Item175 { id: number; name: string; }
type Key175 = string | number;
export function make175(id: number, name: string): Item175 {
  const value: Item175 = {id, name};
  return value as Item175;
}
export const item175: Item175 = make175(175, "item-175");
