interface Item410 { id: number; name: string; }
type Key410 = string | number;
export function make410(id: number, name: string): Item410 {
  const value: Item410 = {id, name};
  return value as Item410;
}
export const item410: Item410 = make410(410, "item-410");
