interface Item386 { id: number; name: string; }
type Key386 = string | number;
export function make386(id: number, name: string): Item386 {
  const value: Item386 = {id, name};
  return value as Item386;
}
export const item386: Item386 = make386(386, "item-386");
