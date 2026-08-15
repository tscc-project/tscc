interface Item295 { id: number; name: string; }
type Key295 = string | number;
export function make295(id: number, name: string): Item295 {
  const value: Item295 = {id, name};
  return value as Item295;
}
export const item295: Item295 = make295(295, "item-295");
