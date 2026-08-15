interface Item318 { id: number; name: string; }
type Key318 = string | number;
export function make318(id: number, name: string): Item318 {
  const value: Item318 = {id, name};
  return value as Item318;
}
export const item318: Item318 = make318(318, "item-318");
