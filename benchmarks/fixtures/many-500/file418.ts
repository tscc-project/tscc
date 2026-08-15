interface Item418 { id: number; name: string; }
type Key418 = string | number;
export function make418(id: number, name: string): Item418 {
  const value: Item418 = {id, name};
  return value as Item418;
}
export const item418: Item418 = make418(418, "item-418");
