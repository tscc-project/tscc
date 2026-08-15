interface Item146 { id: number; name: string; }
type Key146 = string | number;
export function make146(id: number, name: string): Item146 {
  const value: Item146 = {id, name};
  return value as Item146;
}
export const item146: Item146 = make146(146, "item-146");
