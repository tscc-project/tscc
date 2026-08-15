interface Item209 { id: number; name: string; }
type Key209 = string | number;
export function make209(id: number, name: string): Item209 {
  const value: Item209 = {id, name};
  return value as Item209;
}
export const item209: Item209 = make209(209, "item-209");
