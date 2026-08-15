interface Item264 { id: number; name: string; }
type Key264 = string | number;
export function make264(id: number, name: string): Item264 {
  const value: Item264 = {id, name};
  return value as Item264;
}
export const item264: Item264 = make264(264, "item-264");
