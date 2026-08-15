interface Item273 { id: number; name: string; }
type Key273 = string | number;
export function make273(id: number, name: string): Item273 {
  const value: Item273 = {id, name};
  return value as Item273;
}
export const item273: Item273 = make273(273, "item-273");
